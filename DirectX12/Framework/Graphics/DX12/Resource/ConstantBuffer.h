#pragma once
#include <d3d12.h>
#include "Framework/Utility/Typedef.h"
#include "Framework/Utility/Debug.h"

namespace Framework {
namespace Graphics {

/**
* @class ConstantBuffer
* @brief コンスタントバッファ管理
*/
class ConstantBuffer {
public:
    /**
    * @brief コンストラクタ
    */
    ConstantBuffer(UINT reservationNum);
    /**
    * @brief デストラクタ
    */
    ~ConstantBuffer();

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
    ComPtr<ID3D12Resource> mConstantBuffer; //!< バッファリソース
    ComPtr<ID3D12DescriptorHeap> mCBVHeap; //!< ヒープ
    UINT mAssignedNum; //!< 現在割り当てている数
    UINT mOffset; //!< バッファのメモリオフセット
    UINT mReservationNum; //!< コンスタントバッファの割り当て予約数
    struct { char buf[256]; }*mCBVDataBegin; //!< メモリの開始地点
    bool mUpdateFlag;
};

} //Graphics 
} //Framework 