#pragma once
#include "Framework/Math/Vector2.h"

namespace Framework {
namespace Math {

/**
* @class AABB2D
* @brief ��`
*/
class AABB2D {
public:
    /**
    * @brief �R���X�g���N�^
    */
    AABB2D() :AABB2D(Vector2::ZERO, Vector2::ZERO) { }
    /**
    * @brief �R���X�g���N�^
    * @param position ���S���W
    * @param size ���ƍ���
    */
    AABB2D(const Vector2& position, const Vector2& size)
        :position(position), size(size) { }
    /**
    * @brief �R���X�g���N�^
    * @param left �����W(_x)
    * @param top ����W(_y)
    * @param right �E���W(_x)
    * @param bottom �����W(_y)
    */
    AABB2D(float left, float top, float right, float bottom)
        :AABB2D(Vector2((left + right) * 0.5f, (top + bottom) * 0.5f),
            Vector2(right - left, bottom - top)) { }
    /**
    * @brief ��
    */
    inline float left() const {
        return position.x - size.x * 0.5f;
    }
    /**
    * @brief �E
    */
    inline float right() const {
        return position.x + size.x * 0.5f;
    }
    /**
    * @brief ��
    */
    inline float top() const {
        return position.y - size.y * 0.5f;
    }
    /**
    * @brief ��
    */
    inline float bottom() const {
        return position.y + size.y * 0.5f;
    }
public:
    Vector2 position; //!< ���S���W
    Vector2 size; //!< ���ƍ���
};

} //Math 
} //Framework 
