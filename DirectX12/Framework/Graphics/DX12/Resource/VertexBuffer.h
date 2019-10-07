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
* @brief ���_�o�b�t�@
*/
class VertexBuffer {
public:
    /**
    * @brief �R���X�g���N�^
    */
    template <class T>
    VertexBuffer(const std::vector<T>& vertices);
    /**
    * @brief �f�X�g���N�^
    */
    ~VertexBuffer();
    /**
    * @brief �R�}���h���X�g�ɓo�^����
    */
    void addToCommandList(ID3D12GraphicsCommandList* commandList) const;
private:
    const UINT mVertexBufferSize; //!< ���_�f�[�^�̃������T�C�Y
    ComPtr<ID3D12Resource> mVertexBuffer; //!< ���_�o�b�t�@
    D3D12_VERTEX_BUFFER_VIEW mVertexBufferView; //!< ���_�o�b�t�@�r���[
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

    //���_�f�[�^���R�s�[����
    UINT8* vertexDataBegin;
    D3D12_RANGE range{ 0,0 };
    throwIfFailed(mVertexBuffer->Map(0, &range, reinterpret_cast<void**>(&vertexDataBegin)));
    memcpy(vertexDataBegin, vertices.data(), mVertexBufferSize);
    mVertexBuffer->Unmap(0, nullptr);

    //�r���[�ɏ���ۑ����Ă���
    mVertexBufferView.BufferLocation = mVertexBuffer->GetGPUVirtualAddress();
    mVertexBufferView.StrideInBytes = sizeof(T);
    mVertexBufferView.SizeInBytes = mVertexBufferSize;
}

} //Graphics 
} //Framework 