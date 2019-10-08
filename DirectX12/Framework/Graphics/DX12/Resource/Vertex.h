#pragma once
#include "Framework/Math/Vector2.h"
#include "Framework/Math/Vector3.h"
#include "Framework/Math/Vector4.h"
#include "Framework/Graphics/Color4.h"

namespace Framework {
namespace Graphics {

/**
* @brief 頂点座標のみ
*/
struct OnlyPosition {
    Framework::Math::Vector4 pos;
};

/**
* @brief モデルの頂点情報
*/
struct ModelVertex {
    Framework::Math::Vector4 pos;
    Framework::Math::Vector3 normal;
    Framework::Math::Vector2 uv;
    Framework::Graphics::Color4 color;
};

/**
* @brief 頂点座標と法線情報
*/
struct PositionNormal {
    Framework::Math::Vector4 pos;
    Framework::Math::Vector3 normal;
};

} //Graphics 
} //Framework 