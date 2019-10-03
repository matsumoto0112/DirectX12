#pragma once
#include <d3d12.h>

namespace Framework {
namespace Graphics {

/**
* @class IDX12Resource
* @brief DirectX12のリソースインターフェース
*/
class IDX12Resource {
public:
    /**
    * @brief デストラクタ
    */
    virtual ~IDX12Resource() = default;
    /**
    * @brief コマンドリストに追加する
    */
    virtual void addToCommandList(ID3D12GraphicsCommandList* commandList) const = 0;
};

} //Graphics 
} //Framework 