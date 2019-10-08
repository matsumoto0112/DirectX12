#include "DX12Manager.h"
#include "Framework/Define/Debug.h"
#include "Framework/Define/Path.h"
#include "Framework/Define/Render.h"
#include "Framework/Utility/IO/ShaderReader.h"
#include "Framework/Graphics/DX12/Desc/BlendState.h"
#include "Framework/Graphics/DX12/Desc/Rasterizer.h"
#include "Framework/Graphics/DX12/Desc/Sampler.h"
#include "Framework/Graphics/DX12/Helper.h"
#include "Framework/Utility/IO/ByteReader.h"

namespace {
/**
* @brief ハードウェアアダプターを取得する
*/
_Use_decl_annotations_
void GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter) {
    ComPtr<IDXGIAdapter1> adapter;
    *ppAdapter = nullptr;

    for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex) {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);

        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
            // Don't select the Basic Render Driver adapter.
            // If you want a software adapter, pass in "/warp" on the command line.
            continue;
        }

        // Check to see if the adapter supports Direct3D 12, but don't create the
        // actual device yet.
        if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr))) {
            break;
        }
    }

    *ppAdapter = adapter.Detach();
}
}

namespace Framework {
namespace Graphics {

DX12Manager::DX12Manager(HWND hWnd, UINT width, UINT height, UINT frameCount)
    :mHWnd(hWnd), mWidth(width), mHeight(height) {
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

    //ファクトリの作成
    ComPtr<IDXGIFactory4> factory;
    throwIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));
    static const bool useWarpDevice = true;
    if (useWarpDevice) {
        ComPtr<IDXGIAdapter> warpAdapter;
        throwIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

        throwIfFailed(D3D12CreateDevice(
            warpAdapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&mDevice)
        ));
    }
    else {
        ComPtr<IDXGIAdapter1> hardwareAdapter;
        GetHardwareAdapter(factory.Get(), &hardwareAdapter);

        throwIfFailed(D3D12CreateDevice(
            hardwareAdapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&mDevice)
        ));
    }

    //キュー作成
    D3D12_COMMAND_QUEUE_DESC queueDesc{};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    throwIfFailed(mDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue)));
    NAME_D3D12_OBJECT(mCommandQueue);

    //スワップチェイン作成
    DXGI_SWAP_CHAIN_DESC1 desc{};
    desc.BufferCount = FRAME_COUNT;
    desc.Width = mWidth;
    desc.Height = mHeight;
    desc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_DISCARD;
    desc.SampleDesc.Count = 1;

    ComPtr<IDXGISwapChain1> sc;
    throwIfFailed(factory->CreateSwapChainForHwnd(
        mCommandQueue.Get(),
        mHWnd,
        &desc,
        nullptr,
        nullptr,
        &sc));

    //フルスクリーン禁止
    throwIfFailed(factory->MakeWindowAssociation(mHWnd, DXGI_MWA_NO_ALT_ENTER));

    //スワップチェインを1から3に変換する
    throwIfFailed(sc.As(&mSwapChain));
    mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();

    //コマンドアロケータをフレーム数分作成
    for (UINT n = 0; n < FRAME_COUNT; n++) {
        throwIfFailed(mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocator[n])));
        NAME_D3D12_OBJECT_INDEXED(mCommandAllocator, n);
    }

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

DX12Manager::~DX12Manager() {
    waitForPreviousFrame();
    CloseHandle(mFenceEvent);
}

void DX12Manager::createDefaultAsset() {
    //RTVの作成
    D3D12_DESCRIPTOR_HEAP_DESC rtv{};
    rtv.NumDescriptors = FRAME_COUNT;
    rtv.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtv.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    throwIfFailed(mDevice->CreateDescriptorHeap(&rtv, IID_PPV_ARGS(&mRTVHeap)));

    //DSVの作成
    D3D12_DESCRIPTOR_HEAP_DESC dsv{};
    dsv.NumDescriptors = 1;
    dsv.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsv.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    throwIfFailed(mDevice->CreateDescriptorHeap(&dsv, IID_PPV_ARGS(&mDSVHeap)));

    mRTVDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    //フレーム数分のレンダーターゲットを作成
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRTVHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT n = 0; n < FRAME_COUNT; n++) {
        throwIfFailed(mSwapChain->GetBuffer(n, IID_PPV_ARGS(&mRenderTargets[n])));
        mDevice->CreateRenderTargetView(mRenderTargets[n].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, mRTVDescriptorSize);

        NAME_D3D12_OBJECT_INDEXED(mRenderTargets, n);
    }

    //ルートシグネチャ作成
    mRootSignature = std::make_shared<RootSignature>();
    mRootSignature->addStaticSamplerParameter(Sampler::createStaticSampler(FilterMode::Linear, AddressMode::Clamp, VisibilityType::All, 0));
    mRootSignature->createDefaultRootSignature();

    //パイプライン作成
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

    Framework::Utility::ShaderReader vsReader((std::string)Framework::Define::Path::getInstance().shader + "VertexShader.cso");
    Framework::Utility::ShaderReader psReader((std::string)Framework::Define::Path::getInstance().shader + "PixelShader.cso");
    std::vector<BYTE> vs = vsReader.get();
    std::vector<D3D12_INPUT_ELEMENT_DESC> elem = vsReader.getShaderReflection();
    std::vector<BYTE> ps = psReader.get();

    mDefaultPipeline = std::make_unique<Pipeline>(mRootSignature);
    mDefaultPipeline->setVertexShader({ vs.data(),vs.size() });
    mDefaultPipeline->setPixelShader({ ps.data(),ps.size() });
    mDefaultPipeline->setInputLayout({ elem.data(),(UINT)elem.size() });
    mDefaultPipeline->setBlendState(createBlendState());
    mDefaultPipeline->setRasterizerState(Rasterizer(FillMode::Solid, CullMode::Back));
    mDefaultPipeline->setPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
    mDefaultPipeline->setSampleDesc({ 1,0 });
    mDefaultPipeline->setSampleMask(UINT_MAX);
    mDefaultPipeline->setDepthStencil(CD3DX12_DEPTH_STENCIL_DESC1(D3D12_DEFAULT), DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT);
    mDefaultPipeline->setRenderTarget({ DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM });
    mDefaultPipeline->createPipelineState();

    throwIfFailed(mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator[mFrameIndex].Get(), mDefaultPipeline->getPipelineState(), IID_PPV_ARGS(&mCommandList)));
    NAME_D3D12_OBJECT(mCommandList);

    //DSV作成
    D3D12_DEPTH_STENCIL_VIEW_DESC desc{};
    desc.Format = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
    desc.ViewDimension = D3D12_DSV_DIMENSION::D3D12_DSV_DIMENSION_TEXTURE2D;
    desc.Flags = D3D12_DSV_FLAGS::D3D12_DSV_FLAG_NONE;

    D3D12_CLEAR_VALUE val{};
    val.Format = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
    val.DepthStencil.Depth = 1.0f;
    val.DepthStencil.Stencil = 0;

    throwIfFailed(mDevice->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT, mWidth, mHeight, 1, 0, 1, 0, D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
        D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &val,
        IID_PPV_ARGS(&mDepthStencil)));

    NAME_D3D12_OBJECT(mDepthStencil);

    mDevice->CreateDepthStencilView(mDepthStencil.Get(), &desc, mDSVHeap->GetCPUDescriptorHandleForHeapStart());

    //フェンス作成
    throwIfFailed(mDevice->CreateFence(mFenceValue[mFrameIndex], D3D12_FENCE_FLAGS::D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)));
    mFenceValue[mFrameIndex]++;

    mFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (mFenceEvent == nullptr) {
        throwIfFailed(HRESULT_FROM_WIN32(GetLastError()));
    }
}

void DX12Manager::drawBegin() {
    throwIfFailed(mCommandAllocator[mFrameIndex]->Reset());
    throwIfFailed(mCommandList->Reset(mCommandAllocator[mFrameIndex].Get(), mDefaultPipeline->getPipelineState()));

    mRootSignature->addToCommandList(mCommandList.Get());
    mDefaultPipeline->addToCommandList(mCommandList.Get());

    mCommandList->RSSetViewports(1, &mViewport);
    mCommandList->RSSetScissorRects(1, &mScissorRect);

    mCommandList->ResourceBarrier(1, &createResourceBarrier(mRenderTargets[mFrameIndex].Get(), D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET));
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(mRTVHeap->GetCPUDescriptorHandleForHeapStart(), mFrameIndex, mRTVDescriptorSize);
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsv(mDSVHeap->GetCPUDescriptorHandleForHeapStart());
    mCommandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);

    const float clear[] = { 0.0f,0.0f,0.0f,1.0f };
    mCommandList->ClearRenderTargetView(rtv, clear, 0, nullptr);
    mCommandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAGS::D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAGS::D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
}

void DX12Manager::drawEnd() {
    mCommandList->ResourceBarrier(1, &createResourceBarrier(mRenderTargets[mFrameIndex].Get(), D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT));

    executeCommand();

    throwIfFailed(mSwapChain->Present(1, 0));
    waitForPreviousFrame();
}

void DX12Manager::executeCommand() {
    throwIfFailed(mCommandList->Close());
    ID3D12CommandList* lists[] = { mCommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(lists), lists);
}

void DX12Manager::waitForPreviousFrame() {
    const UINT64 fence = mFenceValue[mFrameIndex];
    throwIfFailed(mCommandQueue->Signal(mFence.Get(), fence));
    mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();
    if (mFence->GetCompletedValue() < fence) {
        throwIfFailed(mFence->SetEventOnCompletion(fence, mFenceEvent));
        WaitForSingleObjectEx(mFenceEvent, INFINITE, FALSE);
    }
    mFenceValue[mFrameIndex] = fence + 1;
}

} //Graphics 
} //Framework 
