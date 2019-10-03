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
private:
    ComPtr<ID3D12DescriptorHeap> mSRVHeap; //!< テクスチャSRV用
    ComPtr<ID3D12Resource> mTexture; //!< テクスチャ
};

} //Graphics 
} //Framework 