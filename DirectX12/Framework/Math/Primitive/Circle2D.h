#pragma once
#include "Framework/Math/Vector2.h"

namespace Framework {
namespace Math {

/**
* @class Circle2D
* @brief 円
*/
class Circle2D {
public:
    /**
    * @brief デフォルトコンストラクタ
    */
    Circle2D() :Circle2D(Vector2::ZERO, 1.0f) { };
    /**
    * @brief コンストラクタ
    * @param center 中心座標
    * @param radius 半径
    */
    Circle2D(const Vector2& center, float radius)
        :center(center), radius(radius) { };
    /**
    * @brief デストラクタ
    */
    ~Circle2D() { };
public:
    Vector2 center; //!< 中心座標
    float radius; //!< 半径
};

} //Math 
} //Framework 
