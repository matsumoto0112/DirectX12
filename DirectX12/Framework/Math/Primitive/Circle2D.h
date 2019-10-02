#pragma once
#include "Framework/Math/Vector2.h"

namespace Framework {
namespace Math {

/**
* @class Circle2D
* @brief �~
*/
class Circle2D {
public:
    /**
    * @brief �f�t�H���g�R���X�g���N�^
    */
    Circle2D() :Circle2D(Vector2::ZERO, 1.0f) { };
    /**
    * @brief �R���X�g���N�^
    * @param center ���S���W
    * @param radius ���a
    */
    Circle2D(const Vector2& center, float radius)
        :center(center), radius(radius) { };
    /**
    * @brief �f�X�g���N�^
    */
    ~Circle2D() { };
public:
    Vector2 center; //!< ���S���W
    float radius; //!< ���a
};

} //Math 
} //Framework 
