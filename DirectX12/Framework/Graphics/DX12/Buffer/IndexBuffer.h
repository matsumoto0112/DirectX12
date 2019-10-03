#pragma once
#include <vector>
#include <d3d12.h>
#include "Framework/Graphics/DX12/IDX12Resource.h"
#include "Framework/Utility/Typedef.h"

namespace Framework {
namespace Graphics {

/**
* @class IndexBuffer
* @brief インデックスバッファ
*/
class IndexBuffer : public IDX12Resource {
public:
    /**
    * @brief コンストラクタ
    */
    IndexBuffer(const std::vector<UINT>& indices);
    /**
    * @brief デストラクタ
    */
    virtual ~IndexBuffer();
    /**
    * @brief コマンドリストに自身を追加する
    */
    virtual void addToCommandList(ID3D12GraphicsCommandList* commandList) const override;
    /**
    * @brief 描画指令を送る
    */
    virtual void drawCall(ID3D12GraphicsCommandList* commandList);
private:
    const UINT mIndexNum; //!< インデックスの数
    ComPtr<ID3D12Resource> mIndexBuffer; //!< インデックスバッファ
    D3D12_INDEX_BUFFER_VIEW mIndexBufferView; //!< インデックスバッファビュー
};

} //Graphics 
} //Framework 