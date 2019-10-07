#pragma once
#include <vector>
#include <d3d12.h>
#include "Framework/Graphics/DX12/DXInterfaceAccessor.h"
#include "Framework/Graphics/DX12/Helper.h"
#include "Framework/Utility/Typedef.h"

namespace Framework {
namespace Graphics {

/**
* @class VertexBuffer
* @brief 頂点バッファ
*/
class VertexBuffer {
public:
    /**
    * @brief コンストラクタ
    */
    template <class T>
    VertexBuffer(const std::vector<T>& vertices);
    /**
    * @brief デストラクタ
    */
    ~VertexBuffer();
    /**
    * @brief コマンドリストに登録する
    */
    void addToCommandList(ID3D12GraphicsCommandList* commandList) const;
private:
    const UINT mVertexBufferSize; //!< 頂点データのメモリサイズ
    ComPtr<ID3D12Resource> mVertexBuffer; //!< 頂点バッファ
    D3D12_VERTEX_BUFFER_VIEW mVertexBufferView; //!< 頂点バッファビュー
};

template<class T>
inline VertexBuffer::VertexBuffer(const std::vector<T>& vertices)
    :mVertexBufferSize(vertices.size() * sizeof(T)) {
    throwIfFailed(DXInterfaceAccessor::getDevice()->CreateCommittedResource(
        &createProperty(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
        &createResource(mVertexBufferSize),
        D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&mVertexBuffer)));

    //頂点データをコピーする
    UINT8* vertexDataBegin;
    D3D12_RANGE range{ 0,0 };
    throwIfFailed(mVertexBuffer->Map(0, &range, reinterpret_cast<void**>(&vertexDataBegin)));
    memcpy(vertexDataBegin, vertices.data(), mVertexBufferSize);
    mVertexBuffer->Unmap(0, nullptr);

    //ビューに情報を保存しておく
    mVertexBufferView.BufferLocation = mVertexBuffer->GetGPUVirtualAddress();
    mVertexBufferView.StrideInBytes = sizeof(T);
    mVertexBufferView.SizeInBytes = mVertexBufferSize;
}

} //Graphics 
} //Framework 