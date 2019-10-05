#include "DX12Manager.h"
#include "Framework/Utility/IO/ByteReader.h"
#include "Framework/Graphics/DX12/Helper.h"
#include "Framework/Define/Path.h"
#include "Framework/Utility/IO/ShaderReader.h"
#include "Framework/Graphics/DX12/Desc/Sampler.h"
#include "Framework/Graphics/DX12/Desc/Rasterizer.h"
#include "Framework/Graphics/DX12/Desc/BlendState.h"

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

    D3D12_STATIC_SAMPLER_DESC sampler = Sampler::createStaticSampler(FilterMode::Linear, AddressMode::Wrap, VisibilityType::Pixel, 0);

    //mRootSignature = std::make_shared<RootSignature>();
    //mRootSignature->addConstantBufferParameter(VisibilityType::All, 0);
    //mRootSignature->addTextureParameter(VisibilityType::All, 0);
    //mRootSignature->addConstantBufferParameter(VisibilityType::All, 1);
    //mRootSignature->getRootSignature()

    auto  createRange = [](D3D12_DESCRIPTOR_RANGE_TYPE type, UINT num, UINT baseRegisterNumber, UINT registerSpace, D3D12_DESCRIPTOR_RANGE_FLAGS flag, UINT offset = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND) {
        D3D12_DESCRIPTOR_RANGE1 range{};
        range.RangeType = type;
        range.NumDescriptors = num;
        range.BaseShaderRegister = baseRegisterNumber;
        range.RegisterSpace = registerSpace;
        range.Flags = flag;
        range.OffsetInDescriptorsFromTableStart = offset;
        return range;
    };

    auto createRootSignatureDesc = [](UINT num,
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


    constexpr UINT CONSTANT_BUFFER_NUM = 4;
    constexpr UINT TEXTURE_MAX_NUM = 1;
    D3D12_VERSIONED_ROOT_SIGNATURE_DESC vrsd;
    D3D12_DESCRIPTOR_RANGE1 CBRange[1];
    CBRange[0] = createRange(D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
        CONSTANT_BUFFER_NUM, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAGS::D3D12_DESCRIPTOR_RANGE_FLAG_NONE);
    D3D12_DESCRIPTOR_RANGE1 TEXRange[1];
    TEXRange[0] = createRange(D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
        TEXTURE_MAX_NUM, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAGS::D3D12_DESCRIPTOR_RANGE_FLAG_NONE);

    D3D12_ROOT_PARAMETER1 Pamameter[2]{};
    Pamameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    Pamameter[0].DescriptorTable.NumDescriptorRanges = 1;
    Pamameter[0].DescriptorTable.pDescriptorRanges = &CBRange[0];
    Pamameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL;

    Pamameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    Pamameter[1].DescriptorTable.NumDescriptorRanges = 1;
    Pamameter[1].DescriptorTable.pDescriptorRanges = &TEXRange[0];
    Pamameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL;

    vrsd = createRootSignatureDesc(_countof(Pamameter), Pamameter, 1, &sampler, D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
    ComPtr<ID3DBlob> sigunature, error;
    throwIfFailed(D3D12SerializeVersionedRootSignature(&vrsd, &sigunature, &error));

    throwIfFailed(DXInterfaceAccessor::getDevice()->CreateRootSignature(0,
        sigunature->GetBufferPointer(),
        sigunature->GetBufferSize(),
        IID_PPV_ARGS(&mRootSignature)));

    {
        auto createBlendState = []() {
            D3D12_BLEND_DESC blendDesc{};
            blendDesc.AlphaToCoverageEnable = FALSE;
            blendDesc.IndependentBlendEnable = FALSE;
            const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc = BlendState::defaultBlendDesc();
            for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++) {
                blendDesc.RenderTarget[i] = defaultRenderTargetBlendDesc;
            }

            return blendDesc;
        };

        Framework::Utility::ShaderReader vsreader((std::string)Framework::Define::Path::getInstance().shader + "VertexShader.cso");
        std::vector<BYTE> vs = vsreader.get();
        std::vector<D3D12_INPUT_ELEMENT_DESC> elemDescs = vsreader.getShaderReflection();
        Framework::Utility::ShaderReader psreader((std::string)Framework::Define::Path::getInstance().shader + "PixelShader.cso");
        std::vector<BYTE> ps = psreader.get();

        //mDefaultPipeline = std::make_unique<Pipeline>(mRootSignature);
        mDefaultPipeline = std::make_unique<Pipeline>();
        mDefaultPipeline->mPSODesc.pRootSignature = mRootSignature.Get();
        mDefaultPipeline->setVertexShader({ vs.data(),vs.size() });
        mDefaultPipeline->setPixelShader({ ps.data(),ps.size() });
        mDefaultPipeline->setInputLayout({ elemDescs.data(),(UINT)elemDescs.size() });
        mDefaultPipeline->setBlendState(createBlendState());
        mDefaultPipeline->setRasterizerState(Rasterizer(FillMode::Solid, CullMode::None));
        mDefaultPipeline->setPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
        mDefaultPipeline->setSampleDesc({ 1,0 });
        mDefaultPipeline->setSampleMask(UINT_MAX);
        mDefaultPipeline->setRenderTarget({ DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM });
        mDefaultPipeline->createPipelineState();


        throwIfFailed(mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), mDefaultPipeline->getPipelineState(), IID_PPV_ARGS(&mCommandList)));

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

    throwIfFailed(mDevice->CreateFence(0, D3D12_FENCE_FLAGS::D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)));
    mFenceValue = 1;
    mFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!mFenceEvent) {
        throwIfFailed(HRESULT_FROM_WIN32(GetLastError()));
    }
}

void DX12Manager::finalize() {
    waitForPreviousFrame();
    CloseHandle(mFenceEvent);
}

void DX12Manager::drawBegin() {
    throwIfFailed(mCommandAllocator->Reset());
    throwIfFailed(mCommandList->Reset(mCommandAllocator.Get(), mDefaultPipeline->getPipelineState()));

    //mRootSignature->addToCommandList(mCommandList.Get());
    mCommandList->SetGraphicsRootSignature(mRootSignature.Get());
    mDefaultPipeline->addToCommandList(mCommandList.Get());

    mCommandList->RSSetViewports(1, &mViewport);
    mCommandList->RSSetScissorRects(1, &mScissorRect);

    mCommandList->ResourceBarrier(1, &createResourceBarrier(mRenderTargets[mFrameIndex].Get(), D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET));
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{};
    rtvHandle.ptr = static_cast<SIZE_T>(mRTVHeap->GetCPUDescriptorHandleForHeapStart().ptr + INT64(mFrameIndex) * UINT64(mRTVDescriptorSize));
    mCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    const float clear[] = { 0.0f,0.0f,0.0f,0.0f };
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
