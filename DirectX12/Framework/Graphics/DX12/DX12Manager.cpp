#include "DX12Manager.h"
#include "Framework/Utility/IO/ByteReader.h"
#include "Framework/Graphics/DX12/Helper.h"
#include "Framework/Define/Path.h"

namespace {
/**
* @brief ハードウェアアダプターを取得する
*/
void getHardwareAdapter(IDXGIFactory2* factory, IDXGIAdapter1** ppAdapter) {
    ComPtr<IDXGIAdapter1> adapter = nullptr;
    *ppAdapter = nullptr;

    for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(adapterIndex, &adapter); adapterIndex++) {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);

        if (desc.Flags & DXGI_ADAPTER_FLAG::DXGI_ADAPTER_FLAG_SOFTWARE) {
            continue;
        }

        if (SUCCEEDED(D3D12CreateDevice(
            adapter.Get(),
            D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0,
            __uuidof(ID3D12Device),
            nullptr))) {
            break;
        }
    }

    *ppAdapter = adapter.Detach();
}
}

namespace Framework {
namespace Graphics {

DX12Manager::DX12Manager() { }

DX12Manager::~DX12Manager() { }

void DX12Manager::initialize(HWND hWnd, UINT width, UINT height) {
    //デバッグ用インターフェースを先に作成する
    UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
    {
        ComPtr<ID3D12Debug> debug;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)))) {
            debug->EnableDebugLayer();
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
    }
#endif

    //ファクトリの生成
    ComPtr<IDXGIFactory4> factory;
    throwIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));
    static constexpr bool useWrapDevice = false;
    if (useWrapDevice) {
        ComPtr<IDXGIAdapter> adapter = nullptr;
        throwIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter)));
        throwIfFailed(D3D12CreateDevice(
            adapter.Get(),
            D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&mDevice)));
    }
    else {
        ComPtr<IDXGIAdapter1> adapter;
        getHardwareAdapter(factory.Get(), &adapter);
        throwIfFailed(D3D12CreateDevice(
            adapter.Get(),
            D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&mDevice)));
    }

    //キュー作成
    D3D12_COMMAND_QUEUE_DESC queueDesc{};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAGS::D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT;

    throwIfFailed(mDevice->CreateCommandQueue(
        &queueDesc,
        IID_PPV_ARGS(&mCommandQueue)));

    //スワップチェイン作成
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
    swapChainDesc.BufferCount = FRAME_COUNT;
    swapChainDesc.Width = width;
    swapChainDesc.Height = height;
    swapChainDesc.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    ComPtr<IDXGISwapChain1> swapChain;
    throwIfFailed(factory->CreateSwapChainForHwnd(
        mCommandQueue.Get(),
        hWnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain));
    throwIfFailed(factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));
    throwIfFailed(swapChain.As(&mSwapChain));

    mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();

    //RTV
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
    rtvHeapDesc.NumDescriptors = FRAME_COUNT;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    throwIfFailed(mDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&mRTVHeap)));

    throwIfFailed(mDevice->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(&mCommandAllocator)));

    //RTV作成
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
    rtvHandle = mRTVHeap->GetCPUDescriptorHandleForHeapStart();
    mRTVDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    //それぞれのRTVを作成する
    for (UINT n = 0; n < FRAME_COUNT; n++) {
        throwIfFailed(mSwapChain->GetBuffer(n, IID_PPV_ARGS(&mRenderTargets[n])));
        mDevice->CreateRenderTargetView(mRenderTargets[n].Get(), nullptr, rtvHandle);
        rtvHandle.ptr += INT64(1) * UINT64(mRTVDescriptorSize);
    }

    throwIfFailed(mDevice->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(&mCommandAllocator)));

    D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData{};
    //使用可能なバージョンの中で上位のものを使用する
    featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION::D3D_ROOT_SIGNATURE_VERSION_1_1;
    if (FAILED(mDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData)))) {
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION::D3D_ROOT_SIGNATURE_VERSION_1_0;
    }

    auto initRange = [](D3D12_DESCRIPTOR_RANGE_TYPE type, UINT num, UINT baseRegisterNumber, UINT registerSpace, D3D12_DESCRIPTOR_RANGE_FLAGS flag, UINT offset = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND) {
        D3D12_DESCRIPTOR_RANGE1 range{};
        range.RangeType = type;
        range.NumDescriptors = num;
        range.BaseShaderRegister = baseRegisterNumber;
        range.RegisterSpace = registerSpace;
        range.Flags = flag;
        range.OffsetInDescriptorsFromTableStart = offset;
        return range;
    };

    auto initParam = [](UINT num, const D3D12_DESCRIPTOR_RANGE1* ranges, D3D12_SHADER_VISIBILITY visibility) {
        D3D12_ROOT_PARAMETER1 rootParam{};
        rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootParam.ShaderVisibility = visibility;
        rootParam.DescriptorTable.NumDescriptorRanges = num;
        rootParam.DescriptorTable.pDescriptorRanges = ranges;
        return rootParam;
    };

    auto initRoot = [](UINT num,
        const D3D12_ROOT_PARAMETER1* param,
        UINT numStaticSampler,
        const D3D12_STATIC_SAMPLER_DESC* sampler,
        D3D12_ROOT_SIGNATURE_FLAGS flag) {
            D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc{};
            rootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION::D3D_ROOT_SIGNATURE_VERSION_1;
            rootSignatureDesc.Desc_1_1.NumParameters = num;
            rootSignatureDesc.Desc_1_1.pParameters = param;
            rootSignatureDesc.Desc_1_1.NumStaticSamplers = numStaticSampler;
            rootSignatureDesc.Desc_1_1.pStaticSamplers = sampler;
            rootSignatureDesc.Desc_1_1.Flags = flag;
            return rootSignatureDesc;
    };

    auto serializeVersionedRootSignature = [](
        const D3D12_VERSIONED_ROOT_SIGNATURE_DESC* rootSignatureDesc,
        D3D_ROOT_SIGNATURE_VERSION version,
        ID3DBlob** ppBlob,
        ID3DBlob** ppError) {
            return D3D12SerializeVersionedRootSignature(rootSignatureDesc, ppBlob, ppError);
    };

    D3D12_DESCRIPTOR_RANGE1 range[1];
    range[0] = initRange(D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
        1,
        0,
        0,
        D3D12_DESCRIPTOR_RANGE_FLAGS::D3D12_DESCRIPTOR_RANGE_FLAG_NONE);

    D3D12_ROOT_PARAMETER1 rootParameter[3];
    rootParameter[0] = initParam(1, &range[0], D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL);
    rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameter[1].Descriptor.ShaderRegister = 0;
    rootParameter[1].Descriptor.RegisterSpace = 0;
    rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_VERTEX;
    rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameter[2].Descriptor.ShaderRegister = 1;
    rootParameter[2].Descriptor.RegisterSpace = 0;


    D3D12_ROOT_SIGNATURE_FLAGS  rootSignatureFlags =
        D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    D3D12_STATIC_SAMPLER_DESC sampler{};
    sampler.Filter = D3D12_FILTER::D3D12_FILTER_MIN_MAG_MIP_POINT;
    sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    sampler.MipLODBias = 0;
    sampler.MaxAnisotropy = 0;
    sampler.MinLOD = 0.0f;
    sampler.MaxLOD = D3D12_FLOAT32_MAX;
    sampler.ShaderRegister = 0;
    sampler.RegisterSpace = 0;
    sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_PIXEL;

    D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc = initRoot(
        _countof(rootParameter),
        rootParameter,
        1,
        &sampler,
        rootSignatureFlags);

    ComPtr<ID3DBlob> sigunature, error;
    featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION::D3D_ROOT_SIGNATURE_VERSION_1;
    throwIfFailed(serializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &sigunature, &error));

    HRESULT hr = (mDevice->CreateRootSignature(0,
        sigunature->GetBufferPointer(),
        sigunature->GetBufferSize(),
        IID_PPV_ARGS(&mRootSignature)));

    {
        auto createRasterizerState = []() {
            D3D12_RASTERIZER_DESC rasDesc{};
            rasDesc.FillMode = D3D12_FILL_MODE::D3D12_FILL_MODE_SOLID;
            rasDesc.CullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_BACK;
            rasDesc.FrontCounterClockwise = FALSE;
            rasDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
            rasDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
            rasDesc.DepthClipEnable = TRUE;
            rasDesc.MultisampleEnable = FALSE;
            rasDesc.AntialiasedLineEnable = FALSE;
            rasDesc.ForcedSampleCount = 0;
            rasDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE::D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
            return rasDesc;
        };

        auto createBlendState = []() {
            D3D12_BLEND_DESC blendDesc{};
            blendDesc.AlphaToCoverageEnable = FALSE;
            blendDesc.IndependentBlendEnable = FALSE;
            const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc = {
                FALSE,FALSE,
                D3D12_BLEND::D3D12_BLEND_ONE,D3D12_BLEND::D3D12_BLEND_ZERO,D3D12_BLEND_OP::D3D12_BLEND_OP_ADD,
                D3D12_BLEND::D3D12_BLEND_ONE,D3D12_BLEND::D3D12_BLEND_ZERO,D3D12_BLEND_OP::D3D12_BLEND_OP_ADD,
                D3D12_LOGIC_OP::D3D12_LOGIC_OP_NOOP,
                D3D12_COLOR_WRITE_ENABLE::D3D12_COLOR_WRITE_ENABLE_ALL
            };
            for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++) {
                blendDesc.RenderTarget[i] = defaultRenderTargetBlendDesc;
            }

            return blendDesc;
        };

        {
            //シェーダーの読み込み
            Framework::Utility::ByteReader reader((std::string)Framework::Define::Path::getInstance().shader + "VertexShader.cso");
            std::vector<BYTE> vs = reader.get();
            reader = Framework::Utility::ByteReader((std::string)Framework::Define::Path::getInstance().shader + "PixelShader.cso");
            std::vector<BYTE> ps = reader.get();

            D3D12_INPUT_ELEMENT_DESC elementDescs[] = {
                { "POSITION",   0,DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT,  0,  D3D12_APPEND_ALIGNED_ELEMENT,   D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "TEXCOORD",  0,DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT,        0,  D3D12_APPEND_ALIGNED_ELEMENT,   D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            };

            //パイプライン生成
            D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
            psoDesc.InputLayout = { elementDescs,_countof(elementDescs) };
            psoDesc.pRootSignature = mRootSignature.Get();
            psoDesc.VS.pShaderBytecode = vs.data();
            psoDesc.VS.BytecodeLength = vs.size();
            psoDesc.PS.pShaderBytecode = ps.data();
            psoDesc.PS.BytecodeLength = ps.size();
            psoDesc.RasterizerState = createRasterizerState();
            psoDesc.BlendState = createBlendState();
            psoDesc.DepthStencilState.DepthEnable = FALSE;
            psoDesc.DepthStencilState.StencilEnable = FALSE;
            psoDesc.SampleMask = UINT_MAX;
            psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            psoDesc.NumRenderTargets = 1;
            psoDesc.RTVFormats[0] = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
            psoDesc.SampleDesc.Count = 1;
            throwIfFailed(mDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPipelineState)));
        }
        //コマンドリスト作成
        throwIfFailed(mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), mPipelineState.Get(), IID_PPV_ARGS(&mCommandList)));

        mViewport.TopLeftX = 0;
        mViewport.TopLeftY = 0;
        mViewport.Width = static_cast<float>(width);
        mViewport.Height = static_cast<float>(height);
        mViewport.MinDepth = 0.0f;
        mViewport.MaxDepth = 1.0f;

        mScissorRect.left = 0;
        mScissorRect.top = 0;
        mScissorRect.right = static_cast<LONG>(width);
        mScissorRect.bottom = static_cast<LONG>(height);
    }

    //executeCommand();
}

void DX12Manager::finalize() {
    waitForPreviousFrame();
    CloseHandle(mFenceEvent);
}

void DX12Manager::createFence() {
    //フェンス作成
    {
        throwIfFailed(mDevice->CreateFence(0, D3D12_FENCE_FLAGS::D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)));
        mFenceValue = 1;
        mFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (!mFenceEvent) {
            throwIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }

        waitForPreviousFrame();
    }
}

void DX12Manager::drawBegin() {
    throwIfFailed(mCommandAllocator->Reset());
    throwIfFailed(mCommandList->Reset(mCommandAllocator.Get(), mPipelineState.Get()));

    mCommandList->SetGraphicsRootSignature(mRootSignature.Get());
    mCommandList->SetPipelineState(mPipelineState.Get());

    mCommandList->RSSetViewports(1, &mViewport);
    mCommandList->RSSetScissorRects(1, &mScissorRect);


    mCommandList->ResourceBarrier(1, &createResourceBarrier(mRenderTargets[mFrameIndex].Get(), D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET));
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{};
    rtvHandle.ptr = static_cast<SIZE_T>(mRTVHeap->GetCPUDescriptorHandleForHeapStart().ptr + INT64(mFrameIndex) * UINT64(mRTVDescriptorSize));
    mCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    const float clear[] = { 0.65f,0.2f,0.48f,0.0f };
    mCommandList->ClearRenderTargetView(rtvHandle, clear, 0, nullptr);

}

void DX12Manager::drawEnd() {
    mCommandList->ResourceBarrier(1, &createResourceBarrier(mRenderTargets[mFrameIndex].Get(), D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT));

    executeCommand();

    throwIfFailed(mSwapChain->Present(1, 0));
    waitForPreviousFrame();
}

void DX12Manager::executeCommand() {
    //コマンドリストを閉じて実行処理
    throwIfFailed(mCommandList->Close());
    ID3D12CommandList* lists[] = { mCommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(lists), lists);
}

void DX12Manager::waitForPreviousFrame() {
    const UINT64 fence = mFenceValue;
    throwIfFailed(mCommandQueue->Signal(mFence.Get(), fence));
    mFenceValue++;
    if (mFence->GetCompletedValue() < fence) {
        throwIfFailed(mFence->SetEventOnCompletion(fence, mFenceEvent));
        WaitForSingleObject(mFenceEvent, INFINITE);
    }

    mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();
}

} //Graphics 
} //Framework 
