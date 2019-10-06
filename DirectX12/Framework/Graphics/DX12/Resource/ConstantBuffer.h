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
    /**
    * @brief バッファデータの更新
    */
    template <class T>
    void updateBuffer(const T& buffer);
    /**
    * @brief バッファデータの更新開始
    */
    void beginCBUpdate();
    /**
    * @brief バッファデータの更新終了
    */
    void endCBUpdate(ID3D12GraphicsCommandList* commandList);
    /**
    * @brief 描画フレーム終了時に呼ぶ
    */
    void endFrame();
private:
    /**
    * @brief バッファの更新
    */
    void updateBuffer(void* buf, UINT size);
private:
    ComPtr<ID3D12Resource> mConstantBuffer; //!< バッファリソース
    ComPtr<ID3D12DescriptorHeap> mCBVHeap; //!< ヒープ
    UINT mAssignedNum; //!< 現在割り当てている数
    UINT mOffset; //!< バッファのメモリオフセット
    UINT mReservationNum; //!< コンスタントバッファの割り当て予約数
    struct { char buf[256]; }*mCBVDataBegin; //!< メモリの開始地点
};

template<class T>
inline void ConstantBuffer::updateBuffer(const T& buffer) {
    UINT requiredSize = sizeAlignment(sizeof(buffer));
    UINT requiredNum = requiredSize / 0xff;
    MY_ASSERTION(mAssignedNum + requiredNum <= mReservationNum, "コンスタントバッファの利用可能数を超えています");
    updateBuffer((void*)&buffer, requiredSize);
    mAssignedNum += requiredNum;
}

} //Graphics 
} //Framework 