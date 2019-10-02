#pragma once
#include "Framework/Math/Vector2.h"

namespace Framework {
namespace Math {

/**
* @class AABB2D
* @brief 矩形
*/
class AABB2D {
public:
    /**
    * @brief コンストラクタ
    */
    AABB2D() :AABB2D(Vector2::ZERO, Vector2::ZERO) { }
    /**
    * @brief コンストラクタ
    * @param position 中心座標
    * @param size 幅と高さ
    */
    AABB2D(const Vector2& position, const Vector2& size)
        :position(position), size(size) { }
    /**
    * @brief コンストラクタ
    * @param left 左座標(_x)
    * @param top 上座標(_y)
    * @param right 右座標(_x)
    * @param bottom 下座標(_y)
    */
    AABB2D(float left, float top, float right, float bottom)
        :AABB2D(Vector2((left + right) * 0.5f, (top + bottom) * 0.5f),
            Vector2(right - left, bottom - top)) { }
    /**
    * @brief 左
    */
    inline float left() const {
        return position.x - size.x * 0.5f;
    }
    /**
    * @brief 右
    */
    inline float right() const {
        return position.x + size.x * 0.5f;
    }
    /**
    * @brief 上
    */
    inline float top() const {
        return position.y - size.y * 0.5f;
    }
    /**
    * @brief 下
    */
    inline float bottom() const {
        return position.y + size.y * 0.5f;
    }
public:
    Vector2 position; //!< 中心座標
    Vector2 size; //!< 幅と高さ
};

} //Math 
} //Framework 
