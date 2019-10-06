#include "ShaderResourceView.h"
#include "Framework/Graphics/DX12/DXInterfaceAccessor.h"
#include "Framework/Graphics/DX12/Helper.h"

namespace Framework {
namespace Graphics {

ShaderResourceView::ShaderResourceView(UINT reservationNum)
    :mReservationNum(reservationNum), mCurrentUsedNum(0) {
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
    srvHeapDesc.NumDescriptors = reservationNum;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    throwIfFailed(DXInterfaceAccessor::getDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&mSRVHeap)));
}

ShaderResourceView::~ShaderResourceView() { }

bool ShaderResourceView::canUpdate() const {
    return mCurrentUsedNum < mReservationNum;
}

void ShaderResourceView::updateBuffer(ComPtr<ID3D12Resource> texture) {
    D3D12_CPU_DESCRIPTOR_HANDLE cpuPtr = mSRVHeap->GetCPUDescriptorHandleForHeapStart();
    cpuPtr.ptr += mCurrentUsedNum * DXInterfaceAccessor::getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    DXInterfaceAccessor::getDevice()->CreateShaderResourceView(texture.Get(), nullptr, cpuPtr);
    mCurrentUsedNum++;
}

void ShaderResourceView::beginCBUpdate() {
    mUsedOffset = mCurrentUsedNum;
}

void ShaderResourceView::endCBUpdate(ID3D12GraphicsCommandList* commandList) {
    ID3D12DescriptorHeap* heaps[] = { mSRVHeap.Get(), };
    commandList->SetDescriptorHeaps(_countof(heaps), heaps);

    D3D12_GPU_DESCRIPTOR_HANDLE ptr = mSRVHeap->GetGPUDescriptorHandleForHeapStart();
    ptr.ptr += mUsedOffset * DXInterfaceAccessor::getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    commandList->SetGraphicsRootDescriptorTable(1, ptr);
}

void ShaderResourceView::beginFrame() {
    mCurrentUsedNum = 0;
    mUsedOffset = 0;
}

} //Graphics 
} //Framework 
