#pragma once
#include <stdexcept>
#include <d3d12.h>
#include "Framework/Graphics/DX12/d3dx12.h"

namespace Framework {
namespace Graphics {

/**
* @def オブジェクトに名前を付ける
*/
#define NAME_D3D12_OBJECT(x) SetName((x).Get(),L#x)
#define NAME_D3D12_OBJECT_INDEXED(x,n) SetNameIndexed((x)[n].Get(),L#x,n)


/**
* @brief エラーを文字列に変換する
*/
inline std::string HrToString(HRESULT hr) {
    char s_str[64] = {};
    sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
    return std::string(s_str);
}

/**
* @brief HRESULT例外クラス
*/
class HrException : public std::runtime_error {
public:
    HrException(HRESULT hr) : std::runtime_error(HrToString(hr)), m_hr(hr) { }
    HRESULT error()  const { return m_hr; }
private:
    const HRESULT m_hr;
};

/**
* @brief 失敗したら例外を投げる
*/
inline void throwIfFailed(HRESULT hr) {
    if (FAILED(hr)) {
        throw HrException(hr);
    }
}

/**
* @brief デバッグ用の名前を付ける
*/
#if defined(_DEBUG) || defined(DBG)
inline void SetName(ID3D12Object* pObject, LPCWSTR name) {
    pObject->SetName(name);
}
inline void SetNameIndexed(ID3D12Object* pObject, LPCWSTR name, UINT index) {
    WCHAR fullName[50];
    if (swprintf_s(fullName, L"%s[%u]", name, index) > 0) {
        pObject->SetName(fullName);
    }
}
#else
inline void SetName(ID3D12Object*, LPCWSTR) { }
inline void SetNameIndexed(ID3D12Object*, LPCWSTR, UINT) { }
#endif


/**
* @brief ヒーププロパティを作成する
* @param type ヒープの種類
*/
inline D3D12_HEAP_PROPERTIES createProperty(D3D12_HEAP_TYPE type) {
    D3D12_HEAP_PROPERTIES prop{};
    prop.Type = type;
    prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    prop.MemoryPoolPreference = D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_UNKNOWN;
    prop.CreationNodeMask = 1;
    prop.VisibleNodeMask = 1;
    return prop;
}

/**
* @brief リソースデスクを作成する
* @param size リソースのメモリの大きさ
*/
inline D3D12_RESOURCE_DESC createResource(UINT size) {
    D3D12_RESOURCE_DESC res{};
    res.Dimension = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER;
    res.Alignment = 0;
    res.Width = size;
    res.Height = 1;
    res.DepthOrArraySize = 1;
    res.MipLevels = 1;
    res.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
    res.SampleDesc.Count = 1;
    res.SampleDesc.Quality = 0;
    res.Layout = D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    res.Flags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE;
    return res;
}

/**
* @brief リソースバリアを作成する
*/
inline D3D12_RESOURCE_BARRIER createResourceBarrier(ID3D12Resource* resource,
    D3D12_RESOURCE_STATES before,
    D3D12_RESOURCE_STATES after,
    UINT subResource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
    D3D12_RESOURCE_BARRIER_FLAGS flag = D3D12_RESOURCE_BARRIER_FLAGS::D3D12_RESOURCE_BARRIER_FLAG_NONE) {
    D3D12_RESOURCE_BARRIER result{};
    result.Type = D3D12_RESOURCE_BARRIER_TYPE::D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    result.Flags = flag;
    result.Transition.pResource = resource;
    result.Transition.StateBefore = before;
    result.Transition.StateAfter = after;
    result.Transition.Subresource = subResource;
    return result;
}

/**
* @brief メモリサイズを256の倍数にアラインメント
*/
inline UINT sizeAlignment(UINT size) {
    return ((size + 0xff) & ~0xff);
}

} //Graphics 
} //Framework 