#pragma once
#include <vector>
#include <memory>
#include <d3d12.h>
#include "Framework/Graphics/DX12/Resource/ConstantBuffer.h"
#include "Framework/Utility/Singleton.h"

namespace Framework {
namespace Graphics {
class ConstantBuffer;

/**
* @class ConstantBufferManager
* @brief コンスタントバッファ管理
*/
class ConstantBufferManager {
public:
    /**
    * @brief コンストラクタ
    */
    ConstantBufferManager();
    /**
    * @brief デストラクタ
    */
    ~ConstantBufferManager();
    /**
    * @brief 描画開始時に呼ぶ
    */
    void beginFrame();

    void beingCBufferUpdate();
    void endCBufferUpdate(ID3D12GraphicsCommandList* commandList);
    /**
    * @brief バッファを更新する
    */
    template <class T>
    void updateCBuffer(const T& data);
private:
    /**
    * @brief コンスタントバッファを追加する
    */
    void addConstantBuffer();
private:
    static constexpr UINT CONSTANT_BUFFER_RESERVATION_NUM = 100; //!< コンスタントバッファのメモリ予約数
    std::vector<std::unique_ptr<ConstantBuffer>> mConstantBuffers; //!< コンスタントバッファ
    UINT mCurrentIndex; //!< 現在のコンスタントバッファインデックス
    std::vector<UINT> mBeginedIndex;
};

template<class T>
inline void ConstantBufferManager::updateCBuffer(const T& data) {
    //更新可能な領域があるなら普通に更新する
    if (mConstantBuffers[mCurrentIndex]->canUpdate(sizeof(data))) {
        mConstantBuffers[mCurrentIndex]->updateBuffer((void*)&data, sizeof(T));
    }
    else {
        //ダメだったら次のバッファで試す
        mCurrentIndex++;
        mBeginedIndex.emplace_back(mCurrentIndex);
        //次のバッファがなければ追加する
        if (mCurrentIndex >= mConstantBuffers.size()) {
            addConstantBuffer();
        }
        updateCBuffer(data);
    }
}

} //Graphics 
} //Framework 