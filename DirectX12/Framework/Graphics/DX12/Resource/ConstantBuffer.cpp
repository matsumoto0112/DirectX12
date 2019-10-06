#include "ConstantBuffer.h"
#include "Framework/Graphics/DX12/Helper.h"
#include "Framework/Graphics/DX12/DXInterfaceAccessor.h"

namespace Framework {
namespace Graphics {

ConstantBuffer::ConstantBuffer(UINT reservationNum)
    :mAssignedNum(0), mReservationNum(reservationNum) {
    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc{ };
    cbvHeapDesc.NumDescriptors = reservationNum;
    cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    throwIfFailed(DXInterfaceAccessor::getDevice()->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&mCBVHeap)));
    UINT size = 0x100 * reservationNum;
    throwIfFailed(DXInterfaceAccessor::getDevice()->CreateCommittedResource(
        &createProperty(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
        &createResource(size),
        D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&mConstantBuffer)));

    D3D12_RANGE range{ 0,0 };
    throwIfFailed(mConstantBuffer->Map(0, &range, reinterpret_cast<void**>(&mCBVDataBegin)));
}

ConstantBuffer::~ConstantBuffer() {
    mConstantBuffer->Unmap(0, nullptr);
}

void ConstantBuffer::beginCBUpdate() {
    mOffset = mAssignedNum;
}

void ConstantBuffer::endCBUpdate(ID3D12GraphicsCommandList* commandList) {
    ID3D12DescriptorHeap* heaps[] = { mCBVHeap.Get(), };
    commandList->SetDescriptorHeaps(_countof(heaps), heaps);
    D3D12_GPU_DESCRIPTOR_HANDLE addr = mCBVHeap->GetGPUDescriptorHandleForHeapStart();
    addr.ptr += mOffset * DXInterfaceAccessor::getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    commandList->SetGraphicsRootDescriptorTable(0, addr);
}

void ConstantBuffer::updateBuffer(void* buf, UINT size) {

    memcpy(mCBVDataBegin + mAssignedNum, buf, size);

    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
    cbvDesc.BufferLocation = mConstantBuffer->GetGPUVirtualAddress() + 0x100 * mAssignedNum;
    cbvDesc.SizeInBytes = size;

    D3D12_CPU_DESCRIPTOR_HANDLE ptr = mCBVHeap->GetCPUDescriptorHandleForHeapStart();
    ptr.ptr += mAssignedNum * (DXInterfaceAccessor::getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
    DXInterfaceAccessor::getDevice()->CreateConstantBufferView(&cbvDesc, ptr);
}

void ConstantBuffer::endFrame() {
    mAssignedNum = 0;
}

} //Graphics 
} //Framework 
