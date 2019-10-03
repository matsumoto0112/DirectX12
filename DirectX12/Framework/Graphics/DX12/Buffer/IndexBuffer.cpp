#include "IndexBuffer.h"
#include "Framework/Graphics/DX12/Helper.h"
#include "Framework/Graphics/DX12/DXInterfaceAccessor.h"

namespace Framework {
namespace Graphics {

IndexBuffer::IndexBuffer(const std::vector<UINT>& indices)
    :mIndexNum(indices.size()) {
    const UINT indexBufferSize = sizeof(UINT) * mIndexNum; //インデックスのメモリサイズ
    throwIfFailed(DXInterfaceAccessor::getDevice()->CreateCommittedResource(
        &createProperty(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
        &createResource(indexBufferSize),
        D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&mIndexBuffer)));

    UINT8* indexDataBegin;
    D3D12_RANGE range{ 0,0 };
    throwIfFailed(mIndexBuffer->Map(0, &range, reinterpret_cast<void**>(&indexDataBegin)));
    memcpy(indexDataBegin, indices.data(), indexBufferSize);
    mIndexBuffer->Unmap(0, nullptr);

    mIndexBufferView.BufferLocation = mIndexBuffer->GetGPUVirtualAddress();
    mIndexBufferView.Format = DXGI_FORMAT::DXGI_FORMAT_R32_UINT;
    mIndexBufferView.SizeInBytes = indexBufferSize;
}

IndexBuffer::~IndexBuffer() { }

void IndexBuffer::addToCommandList(ID3D12GraphicsCommandList * commandList) {
    commandList->IASetIndexBuffer(&mIndexBufferView);
}

void IndexBuffer::drawCall(ID3D12GraphicsCommandList * commandList) {
    commandList->DrawIndexedInstanced(mIndexNum, 1, 0, 0, 0);
}

} //Graphics 
} //Framework 
