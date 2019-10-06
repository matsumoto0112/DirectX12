#pragma once
#include <d3d12.h>
#include "Framework/Utility/Typedef.h"
#include "Framework/Utility/Debug.h"

namespace Framework {
namespace Graphics {

/**
* @class ShaderResourceView
* @brief シェーダーリソースビュー
*/
class ShaderResourceView {
public:
    /**
    * @brief コンストラクタ
    */
    ShaderResourceView(UINT reservationNum);
    /**
    * @brief デストラクタ
    */
    ~ShaderResourceView();

    bool canUpdate(UINT size) const;
    /**
    * @brief バッファの更新
    */
    void updateBuffer(void* buf, UINT size);
    /**
    * @brief バッファデータの更新開始
    */
    void beginCBUpdate();
    /**
    * @brief バッファデータの更新終了
    */
    void endCBUpdate(ID3D12GraphicsCommandList* commandList);
    /**
    * @brief 描画フレーム開始時に呼ぶ
    */
    void beginFrame();

private:

};

} //Graphics 
} //Framework 