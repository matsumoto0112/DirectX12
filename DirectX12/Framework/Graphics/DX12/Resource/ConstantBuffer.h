#pragma once
#include <d3d12.h>
#include <typeinfo.h>
#include "Framework/Graphics/DX12/DXInterfaceAccessor.h"
#include "Framework/Graphics/DX12/Helper.h"
#include "Framework/Utility/Debug.h"
#include "Framework/Utility/Typedef.h"

namespace Framework {
namespace Graphics {

/**
* @class ConstantBuffer
* @brief �R���X�^���g�o�b�t�@
*/
class ConstantBuffer {
public:
    /**
    * @brief �R���X�g���N�^
    */
    template <class T>
    ConstantBuffer(const T& bufferStructure);
    /**
    * @brief �f�X�g���N�^
    */
    ~ConstantBuffer();
    /**
    * @brief �o�b�t�@�f�[�^�̍X�V
    */
    template <class T>
    void updateBuffer(const T& data);
    /**
    * @brief �R�}���h���X�g�ɒǉ�����
    */
    void addToCommandList(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex);
private:
    const std::type_info& typeInfo;
    ComPtr<ID3D12DescriptorHeap> mCBVHeap; //!< �R���X�^���g�o�b�t�@�q�[�v
    ComPtr<ID3D12Resource> mConstantBuffer; //!< �R���X�^���g�o�b�t�@
    UINT* mCBVDataBegin;
};

template<class T>
inline ConstantBuffer::ConstantBuffer(const T& bufferStructure)
    :typeInfo(typeid(T)) {
    //CBV
    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc{};
    cbvHeapDesc.NumDescriptors = 1;
    cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    throwIfFailed(DXInterfaceAccessor::getDevice()->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&mCBVHeap)));

    throwIfFailed(DXInterfaceAccessor::getDevice()->CreateCommittedResource(
        &createProperty(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
        &createResource((sizeof(T) + 255) & ~255),
        D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&mConstantBuffer)));

    D3D12_RANGE range{ 0,0 };
    throwIfFailed(mConstantBuffer->Map(0, &range, reinterpret_cast<void**>(&mCBVDataBegin)));
    memcpy(mCBVDataBegin, &bufferStructure, sizeof(T));
}

template<class T>
inline void ConstantBuffer::updateBuffer(const T& data) {
    MY_ASSERTION(typeInfo == typeid(T), "test");
    memcpy(mCBVDataBegin, &data, sizeof(T));
}

} //Graphics 
} //Framework 