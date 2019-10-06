#pragma once
#include <d3d12.h>

namespace Framework {
namespace Graphics {

/**
* @class IMaterial
* @brief マテリアル抽象クラス
*/
class IMaterial {
public:
    /**
    * @brief デストラクタ
    */
    virtual ~IMaterial();
    /**
    * @brief コマンドリストに追加する
    */
    virtual void addToCommandList(ID3D12GraphicsCommandList* commandList);
};

} //Graphics 
} //Framework 