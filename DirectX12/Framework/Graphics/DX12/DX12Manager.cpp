#include "DX12Manager.h"
#include "Framework/Graphics/DX12/Helper.h"
#include "Framework/Define/Path.h"
#include <dxgidebug.h>

namespace {
UINT64 getRequiredIntermediateSize(ID3D12Resource* resource, UINT firstSubResource, UINT subResourceCount) {
    D3D12_RESOURCE_DESC desc = resource->GetDesc();
    UINT64 res = 0;

    ComPtr<ID3D12Device> device;
    resource->GetDevice(IID_PPV_ARGS(&device));
    device->GetCopyableFootprints(&desc,
        firstSubResource, subResourceCount,
        0,
        nullptr,
        nullptr,
        nullptr,
        &res);
    return res;
}

void copySubresource(
    const D3D12_MEMCPY_DEST* dst,
    const D3D12_SUBRESOURCE_DATA* src,
    SIZE_T rowSizeInBytes,
    UINT rowCounts,
    UINT sliceCount
) {
    for (UINT z = 0; z < sliceCount; z++) {
        BYTE* dstSlice = reinterpret_cast<BYTE*>(dst->pData) + dst->SlicePitch * z;
        const BYTE* srcSlice = reinterpret_cast<const BYTE*>(src->pData) + src->SlicePitch * z;

        for (UINT y = 0; y < rowCounts; y++) {
            memcpy(dstSlice + dst->RowPitch * y,
                srcSlice + src->RowPitch * y,
                rowSizeInBytes);
        }
    }
}
UINT64 update(ID3D12GraphicsCommandList* commandList,
    ID3D12Resource* dstResource,
    ID3D12Resource* intermediate,
    UINT firstSubresource,
    UINT subResourceCount,
    UINT64 requiredSize,
    const D3D12_PLACED_SUBRESOURCE_FOOTPRINT* layouts,
    const UINT* numRows,
    const UINT64* rowSizesInBytes,
    const D3D12_SUBRESOURCE_DATA* srcData) {
    D3D12_RESOURCE_DESC imdDesc = intermediate->GetDesc();
    D3D12_RESOURCE_DESC dstDesc = dstResource->GetDesc();

    if (imdDesc.Dimension != D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER ||
        imdDesc.Width < requiredSize + layouts[0].Offset ||
        requiredSize > SIZE_T(-1) ||
        (dstDesc.Dimension == D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER && (firstSubresource != 0))) {
        return 0;
    }

    BYTE* data;
    Framework::Graphics::throwIfFailed(intermediate->Map(0, nullptr, reinterpret_cast<void**>(&data)));

    for (UINT i = 0; i < subResourceCount; i++) {
        if (rowSizesInBytes[i] > SIZE_T(-1))return 0;

        D3D12_MEMCPY_DEST dstData{};
        dstData.pData = data + layouts[i].Offset;
        dstData.RowPitch = layouts[i].Footprint.RowPitch;
        dstData.SlicePitch = layouts[i].Footprint.RowPitch * numRows[i];

        copySubresource(&dstData, &srcData[i], SIZE_T(rowSizesInBytes[i]), numRows[i], layouts[i].Footprint.Depth);
    }

    intermediate->Unmap(0, nullptr);

    if (dstDesc.Dimension == D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER) {
        D3D12_BOX srcBox{};
        srcBox.left = UINT(layouts[0].Offset);
        srcBox.right = UINT(layouts[0].Offset + layouts[0].Footprint.Width);
        srcBox.top = 0;
        srcBox.front = 0;
        srcBox.bottom = 1;
        srcBox.back = 1;

        commandList->CopyBufferRegion(
            dstResource,
            0,
            intermediate,
            layouts[0].Offset,
            layouts[0].Footprint.Width);
    }

    else {
        for (UINT i = 0; i < subResourceCount; i++) {
            D3D12_TEXTURE_COPY_LOCATION dst{};
            D3D12_TEXTURE_COPY_LOCATION src{};

            dst.pResource = dstResource;
            dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
            dst.SubresourceIndex = i + firstSubresource;

            src.pResource = intermediate;
            src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
            src.PlacedFootprint = layouts[i];

            commandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
        }
    }
}


UINT64 update(ID3D12GraphicsCommandList* commandList,
    ID3D12Resource* dstResource,
    ID3D12Resource* intermediate,
    UINT64 intermediateOffset,
    UINT firstSubresource,
    UINT subResourceCount,
    D3D12_SUBRESOURCE_DATA* srcData) {
    UINT64 requiredSize = 0;
    UINT64 bufferSize = static_cast<UINT64>(
        sizeof(D3D12_PLACED_SUBRESOURCE_FOOTPRINT) +
        sizeof(UINT) + sizeof(UINT64) * subResourceCount);
    if (bufferSize > SIZE_MAX)return 0;

    LPVOID buffer = HeapAlloc(GetProcessHeap(), 0, static_cast<SIZE_T>(bufferSize));
    if (!buffer)return 0;

    D3D12_PLACED_SUBRESOURCE_FOOTPRINT* layout = reinterpret_cast<D3D12_PLACED_SUBRESOURCE_FOOTPRINT*>(buffer);
    UINT64* rowSizesInBytes = reinterpret_cast<UINT64*>(layout + subResourceCount);
    UINT* numRows = reinterpret_cast<UINT*>(rowSizesInBytes + subResourceCount);

    D3D12_RESOURCE_DESC desc = dstResource->GetDesc();
    ComPtr<ID3D12Device> device;
    dstResource->GetDevice(IID_PPV_ARGS(&device));
    device->GetCopyableFootprints(
        &desc,
        firstSubresource,
        subResourceCount,
        intermediateOffset,
        layout,
        numRows,
        rowSizesInBytes,
        &requiredSize);

    auto res = update(
        commandList,
        dstResource,
        intermediate,
        firstSubresource,
        subResourceCount,
        requiredSize,
        layout,
        numRows,
        rowSizesInBytes,
        srcData);

    HeapFree(GetProcessHeap(), 0, buffer);
    return res;

}
}

namespace Framework {
namespace Graphics {

void DX12Manager::init(HWND hWnd, UINT width, UINT height) {
    UINT flagsDXGI = 0;
#if _DEBUG
    ComPtr<ID3D12Debug> debug;
    //デバッグ用インターフェース作成
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)))) {
        debug->EnableDebugLayer();
        flagsDXGI |= DXGI_CREATE_FACTORY_DEBUG;
    }
#endif
    //ファクトリ作成
    ComPtr<IDXGIFactory4> factory;
    throwIfFailed(CreateDXGIFactory2(flagsDXGI, IID_PPV_ARGS(&factory)));

    //アダプター作成
    ComPtr<IDXGIAdapter> adapter = nullptr;
    throwIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter)));

    //デバイス作成
    throwIfFailed(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&mDevice)));

    //コマンドキュー作成
    D3D12_COMMAND_QUEUE_DESC cqd;
    ZeroMemory(&cqd, sizeof(cqd));
    cqd.Type = D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT;
    cqd.Priority = 0;
    cqd.Flags = D3D12_COMMAND_QUEUE_FLAGS::D3D12_COMMAND_QUEUE_FLAG_NONE;
    cqd.NodeMask = 0;
    throwIfFailed(mDevice->CreateCommandQueue(&cqd, IID_PPV_ARGS(&mCommandQueue)));

    //スワップチェイン作成
    ComPtr<IDXGISwapChain1> swapChain;
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
    swapChainDesc.BufferCount = FrameCount;
    swapChainDesc.Width = width;
    swapChainDesc.Height = height;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG::DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    throwIfFailed(factory->CreateSwapChainForHwnd(
        mCommandQueue.Get(),
        hWnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain));

    //フルスクリーン非対応にする
    throwIfFailed(factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));

    throwIfFailed(swapChain.As(&mSwapChain));
    mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();

    //ディスクリプタヒープを作成する
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
    ZeroMemory(&heapDesc, sizeof(heapDesc));
    heapDesc.NumDescriptors = FrameCount;;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    throwIfFailed(mDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&mDescriptorHeap)));
    mRTVDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    //レンダーターゲットを作成する
    for (UINT i = 0; i < FrameCount; i++) {
        throwIfFailed(mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mRenderTarget[i])));
        mRTVHandle[i] = mDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
        mRTVHandle[i].ptr += i * mRTVDescriptorSize;
        mDevice->CreateRenderTargetView(mRenderTarget[i].Get(), nullptr, mRTVHandle[i]);
    }

    throwIfFailed(mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocator)));

    mDefaultPipeline = std::make_unique<Pipeline>();
    VertexShader vs((std::string)Define::Path::getInstance().shader + "VertexShader.cso");
    PixelShader ps((std::string)Define::Path::getInstance().shader + "PixelShader.cso");
    mDefaultPipeline->setVertexShader(&vs);
    mDefaultPipeline->setPixelShader(&ps);
    mDefaultPipeline->createPipeline();

    throwIfFailed(mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), mDefaultPipeline->getPipelicneState(), IID_PPV_ARGS(&mCommandList)));
    mCommandList->Close();

    //コマンドキュー用のフェンス作成
    mFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!mFenceEvent) {
        throwIfFailed(HRESULT_FROM_WIN32(GetLastError()));
    }
    throwIfFailed(mDevice->CreateFence(0, D3D12_FENCE_FLAGS::D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mQueueFence)));

    mViewport.TopLeftX = 0;
    mViewport.TopLeftY = 0;
    mViewport.Width = width;
    mViewport.Height = height;
    mViewport.MinDepth = 0.0f;
    mViewport.MaxDepth = 1.0f;

    mScissorRect = { 0,0,static_cast<LONG>(width),static_cast<LONG>(height) };

    //waitForPreviousFrame();
}

ID3D12Device* DX12Manager::getDevice() const {
    return mDevice.Get();
}

void DX12Manager::setPipeline(Pipeline* pipeline) {
    mCurrentPipeline = pipeline;
}

void DX12Manager::begin() {
    throwIfFailed(mCommandAllocator->Reset());
    throwIfFailed(mCommandList->Reset(mCommandAllocator.Get(), mDefaultPipeline->getPipelicneState()));

    mCommandList->SetGraphicsRootSignature(mDefaultPipeline->getRoot());
    barrier(mRenderTarget[mFrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

    float clear[4] = { 1.0f,0.0f,0.0f,0.0f };
    //ビューポート設定
    mCommandList->RSSetViewports(1, &mViewport);
    mCommandList->ClearRenderTargetView(mRTVHandle[mFrameIndex], clear, 0, nullptr);

    mCommandList->RSSetScissorRects(1, &mScissorRect);
    mCommandList->OMSetRenderTargets(1, &mRTVHandle[mFrameIndex], TRUE, nullptr);
    //シェーダーにどのようにデータを渡すかを設定している
    mCommandList->SetGraphicsRootSignature(mDefaultPipeline->getRoot());

    mCommandList->SetPipelineState(mDefaultPipeline->getPipelicneState());
}

void DX12Manager::end() {
    //Present準備
    barrier(mRenderTarget[mFrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    throwIfFailed(mCommandList->Close());


    //コマンドの実行
    Microsoft::WRL::ComPtr<ID3D12CommandList> list = mCommandList.Get();
    mCommandQueue->ExecuteCommandLists(1, list.GetAddressOf());

    mSwapChain->Present(1, 0);

    waitForPreviousFrame();
}

void DX12Manager::finalize() {
    waitForPreviousFrame();
    CloseHandle(mFenceEvent);
}

ID3D12GraphicsCommandList* DX12Manager::getCommandList() const {
    return mCommandList.Get();
}

void DX12Manager::updateSubResource(ComPtr<ID3D12Resource> resource, UINT firstSubResource, UINT subResourceCount, D3D12_SUBRESOURCE_DATA* subData) {
    waitForPreviousFrame();
    ComPtr<ID3D12Resource> intermediate;
    D3D12_RESOURCE_DESC uploadDesc{};
    uploadDesc.Dimension = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER;
    uploadDesc.Alignment = 0;
    uploadDesc.Width = getRequiredIntermediateSize(resource.Get(), firstSubResource, subResourceCount);
    uploadDesc.Height = 1;
    uploadDesc.DepthOrArraySize = 1;
    uploadDesc.MipLevels = 1;
    uploadDesc.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
    uploadDesc.SampleDesc = { 1, 0 };
    uploadDesc.Layout = D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    uploadDesc.Flags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE;

    D3D12_HEAP_PROPERTIES props{};
    props.Type = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD;
    props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    props.MemoryPoolPreference = D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_UNKNOWN;
    props.VisibleNodeMask = 1;
    props.CreationNodeMask = 1;

    throwIfFailed(mDevice->CreateCommittedResource(
        &props,
        D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
        &uploadDesc,
        D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&intermediate)));
    waitForPreviousFrame();
    barrier(resource.Get(), D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST);

    update(mCommandList.Get(), resource.Get(), intermediate.Get(), 0, firstSubResource, subResourceCount, subData);

    barrier(resource.Get(), D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ);

    mCommandList->Close();

    //コマンドの実行
    Microsoft::WRL::ComPtr<ID3D12CommandList> list = mCommandList.Get();
    mCommandQueue->ExecuteCommandLists(1, list.GetAddressOf());

    waitForPreviousFrame();
}

DX12Manager::DX12Manager() { }

DX12Manager::~DX12Manager() { }

void DX12Manager::waitForPreviousFrame() {
    static UINT64 frames = 0;
    mQueueFence->SetEventOnCompletion(frames, mFenceEvent);
    mCommandQueue->Signal(mQueueFence.Get(), frames);
    WaitForSingleObject(mFenceEvent, INFINITE);
    frames++;
    mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();
}

void DX12Manager::barrier(ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) {
    D3D12_RESOURCE_BARRIER barrierDesc;
    ZeroMemory(&barrierDesc, sizeof(barrierDesc));
    barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE::D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrierDesc.Transition.pResource = resource;
    barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrierDesc.Transition.StateBefore = before;
    barrierDesc.Transition.StateAfter = after;
    mCommandList->ResourceBarrier(1, &barrierDesc);
}

} //Graphics 
} //Framework 
