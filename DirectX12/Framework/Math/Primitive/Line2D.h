#pragma once
#include "Framework/Math/Vector2.h"

namespace Framework {
namespace Math {
/**
* @class Line2D
* @brief ����
*/
class Line2D {
public:
    /**
    * @brief �R���X�g���N�^
    */
    Line2D() :Line2D(Vector2::ZERO, Vector2::ZERO) { }

    /**
    * @brief �R���X�g���N�^
    * @param position �n�_
    * @param direction �����x�N�g��
    */
    Line2D(const Vector2& position, const Vector2& direction)
        :position(position), direction(direction) { };
    /**
    * @brief �f�X�g���N�^
    */
    ~Line2D() { };

    /**
    * @brief ���K�����ꂽ�����x�N�g�����擾
    */
    inline Vector2 getNormalDirection() const {
        return direction.getNormal();
    }
public:
    Vector2 position; //!< �n�_
    Vector2 direction; //!< �����x�N�g��
};

} //Math 
} //Framework 
