#pragma once
#include "Framework/Math/Matrix4x4.h"
#include "Framework/Graphics/Color4.h"

namespace Framework {
namespace Graphics {

/**
* @brief MVP�s��o�b�t�@
*/
struct MVPCBuffer {
    Math::Matrix4x4 world; //!< ���[���h�s��
    Math::Matrix4x4 view; //!< �r���[�s��
    Math::Matrix4x4 proj; //!< �v���W�F�N�V�����s��
};

/**
* @brief �F�o�b�t�@
*/
struct ColorCBuffer {
    Color4 color;
};

/**
* @brief UV���o�b�t�@
*/
struct UVCBuffer {
    float left; //!< ����UV���W
    float top; //!< �㑤��UV���W
    float width; //!< UV��
    float height; //!< UV����
};


} //Graphics 
} //Framework 