#pragma once
#include <vector>
#include <d3d12.h>
#include "Framework/Utility/Typedef.h"

namespace Framework {
namespace Graphics {

/**
* @enum PrimitiveTolopolyType
* @brief プリミティブの形状
*/
enum class PrimitiveTolopolyType {
    TriangleList,
    TriangleStrip,
    PointList,
};

/**
* @class IndexBuffer
* @brief インデックスバッファ
*/
class IndexBuffer {
public:
    /**
    * @brief コンストラクタ
    */
    IndexBuffer(const std::vector<UINT>& indices, PrimitiveTolopolyType topologyType);
    /**
    * @brief デストラクタ
    */
    ~IndexBuffer();
    /**
    * @brief コマンドリストに自身を追加する
    */
    void addToCommandList(ID3D12GraphicsCommandList* commandList) const;
    /**
    * @brief 描画指令を送る
    */
    void drawCall(ID3D12GraphicsCommandList* commandList);
private:
    const UINT mIndexNum; //!< インデックスの数
    const D3D_PRIMITIVE_TOPOLOGY mTopologyType; //!< プリミティブの形状
    ComPtr<ID3D12Resource> mIndexBuffer; //!< インデックスバッファ
    D3D12_INDEX_BUFFER_VIEW mIndexBufferView; //!< インデックスバッファビュー
};

} //Graphics 
} //Framework 