#pragma once
#include "Framework/Math/Vector2.h"
#include "Framework/Math/Vector3.h"
#include "Framework/Math/Vector4.h"
#include "Framework/Graphics/Color4.h"

namespace Framework {
namespace Graphics {

/**
* @brief ���_���W�̂�
*/
struct OnlyPosition {
    Framework::Math::Vector4 pos;
};

/**
* @brief ���f���̒��_���
*/
struct ModelVertex {
    Framework::Math::Vector4 pos;
    Framework::Math::Vector3 normal;
    Framework::Math::Vector2 uv;
    Framework::Graphics::Color4 color;
};

/**
* @brief ���_���W�Ɩ@�����
*/
struct PositionNormal {
    Framework::Math::Vector4 pos;
    Framework::Math::Vector3 normal;
};

} //Graphics 
} //Framework 