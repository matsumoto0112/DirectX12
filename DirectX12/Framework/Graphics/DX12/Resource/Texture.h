#pragma once
#include <string>
#include <d3d12.h>
#include "Framework/Utility/Typedef.h"

namespace Framework {
namespace Graphics {

/**
* @class Texture
* @brief テクスチャクラス
*/
class Texture {
public:
    /**
    * @brief コンストラクタ
    */
    Texture(const std::string& filepath);
    /**
    * @brief デストラクタ
    */
    ~Texture();
    /**
    * @brief コマンドリストに追加する
    * @param commandList コマンドリスト
    * @param rootParameterIndex ルートパラメータのインデックス
    */
    void addToCommandList(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex);
    /**
    * @brief ディスクリプタヒープの取得
    */
    ID3D12DescriptorHeap* getDescriptorHeap() const { return mTextureDH.Get(); }
private:
    static constexpr UINT TEXTURE_PIXEL_SIZE = 4; //!< テクスチャのピクセルサイズ
    ComPtr<ID3D12DescriptorHeap> mSRVHeap; //!< テクスチャSRV用
    ComPtr<ID3D12Resource> mTexture; //!< テクスチャ
    ComPtr<ID3D12DescriptorHeap> mTextureDH; //!< テクスチャのディスクリプタヒープ
};

} //Graphics 
} //Framework 