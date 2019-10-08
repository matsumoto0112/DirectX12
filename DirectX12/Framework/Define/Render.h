#pragma once
#include <d3d12.h>

namespace Framework {
namespace Define {

/**
* @class Render
* @brief 描画情報定義
*/
class Render {
public:
    static constexpr UINT FRAME_COUNT = 3;
    static constexpr UINT MAX_CONSTANT_BUFFER_REGISTER_NUM = 16; //!< コンスタントバッファのレジスター最大数
    static constexpr UINT MAX_TEXTURE_REGISTER_NUM = 8; //!< テクスチャのレジスター最大数
    static constexpr UINT MAX_ONE_FRAME_RENDERING_OBJECT = 10000; //!< 1フレームで描画する最大オブジェクト数（同じコンスタントバッファやテクスチャを利用する場合もっと描画可能）
    static constexpr UINT MAX_CONSTANT_BUFFER_USE_NUM_PER_ONE_FRAME = MAX_CONSTANT_BUFFER_REGISTER_NUM * MAX_ONE_FRAME_RENDERING_OBJECT; //!< 1フレームで使用する最大コンスタントバッファ数
    static constexpr UINT MAX_TEXTURE_USE_NUM_PER_ONE_FRAME = MAX_TEXTURE_REGISTER_NUM * MAX_ONE_FRAME_RENDERING_OBJECT; //!< 1フレームで使用する最大コンスタントバッファ数
};

} //Define 
} //Framework 