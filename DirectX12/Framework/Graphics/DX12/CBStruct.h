#pragma once
#include "Framework/Math/Matrix4x4.h"
#include "Framework/Graphics/Color4.h"

namespace Framework {
namespace Graphics {

/**
* @brief MVP行列バッファ
*/
struct MVPCBuffer {
    Math::Matrix4x4 world; //!< ワールド行列
    Math::Matrix4x4 view; //!< ビュー行列
    Math::Matrix4x4 proj; //!< プロジェクション行列
};

/**
* @brief 色バッファ
*/
struct ColorCBuffer {
    Color4 color;
};

/**
* @brief UV情報バッファ
*/
struct UVCBuffer {
    float left; //!< 左のUV座標
    float top; //!< 上側のUV座標
    float width; //!< UV幅
    float height; //!< UV高さ
};


} //Graphics 
} //Framework 