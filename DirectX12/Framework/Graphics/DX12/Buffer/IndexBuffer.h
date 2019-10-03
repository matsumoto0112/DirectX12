#pragma once
#include <vector>
#include <d3d12.h>
#include "Framework/Utility/Typedef.h"

namespace Framework {
namespace Graphics {

/**
* @class IndexBuffer
* @brief インデックスバッファ
*/
class IndexBuffer {
public:
    /**
    * @brief コンストラクタ
    */
    IndexBuffer(const std::vector<UINT>& indices);
    /**
    * @brief デストラクタ
    */
    ~IndexBuffer();
    /**
    * @brief コマンドリストに自身を追加する
    */
    void addToCommandList(ID3D12GraphicsCommandList* commandList);
    /**
    * @brief 描画指令を送る
    */
    void drawCall(ID3D12GraphicsCommandList* commandList);
private:
    const UINT mIndexNum; //!< インデックスの数
    ComPtr<ID3D12Resource> mIndexBuffer; //!< インデックスバッファ
    D3D12_INDEX_BUFFER_VIEW mIndexBufferView; //!< インデックスバッファビュー
};

} //Graphics 
} //Framework 