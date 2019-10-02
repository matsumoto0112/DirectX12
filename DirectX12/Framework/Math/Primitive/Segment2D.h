#pragma once
#include "Framework/Math/Vector2.h"

namespace Framework {
namespace Math {

/**
* @class Segment2D
* @brief ����
*/
class Segment2D {
public:
    /**
    * @brief �R���X�g���N�^
    */
    Segment2D() :Segment2D(Vector2::ZERO, Vector2::ZERO, 0.0f) { };
    /**
    * @brief �R���X�g���N�^
    * @param position �n�_
    * @param direction �����x�N�g���i���K���j
    * @param length ����
    */
    Segment2D(const Vector2& position, const Vector2& direction, float length)
        :position(position), direction(direction), length(length) { };
    /**
    * @brief �R���X�g���N�^
    * @param start �n�_
    * @param end �I�_
    */
    Segment2D(const Vector2& start, const Vector2& end)
        :Segment2D(start, (end - start).getNormal(), (end - start).length()) { }
    /**
    * @brief �I�_�̎擾
    */
    Vector2 getEndPosition() const {
        return position + direction * length;
    };
public:
    Vector2 position; //!< �n�_
    Vector2 direction; //!< �����x�N�g��
    float length; //!< �����̒���
};

} //Math 
} //Framework 
