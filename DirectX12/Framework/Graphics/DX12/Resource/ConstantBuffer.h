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
* @brief コンスタントバッファ
*/
class ConstantBuffer {
public:
    /**
    * @brief コンストラクタ
    */
    template <class T>
    ConstantBuffer(const T& bufferStructure);
    /**
    * @brief デストラクタ
    */
    ~ConstantBuffer();
    /**
    * @brief バッファデータの更新
    */
    template <class T>
    void updateBuffer(const T& data);
    /**
    * @brief コマンドリストに追加する
    */
    void addToCommandList(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex);
private:
    const std::type_info& typeInfo;
    ComPtr<ID3D12DescriptorHeap> mCBVHeap; //!< コンスタントバッファヒープ
    ComPtr<ID3D12Resource> mConstantBuffer; //!< コンスタントバッファ
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