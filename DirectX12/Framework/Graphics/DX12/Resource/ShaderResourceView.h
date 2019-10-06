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

    bool canUpdate() const;
    /**
    * @brief バッファの更新
    */
    void updateBuffer(ComPtr<ID3D12Resource> texture);
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
    ComPtr<ID3D12DescriptorHeap> mSRVHeap;
    const UINT mReservationNum;
    UINT mUsedOffset;
    UINT mCurrentUsedNum;
};

} //Graphics 
} //Framework 