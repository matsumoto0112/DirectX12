#pragma once
#include "Framework/Math/Vector2.h"

namespace Framework {
namespace Math {

/**
* @class Segment2D
* @brief 線分
*/
class Segment2D {
public:
    /**
    * @brief コンストラクタ
    */
    Segment2D() :Segment2D(Vector2::ZERO, Vector2::ZERO, 0.0f) { };
    /**
    * @brief コンストラクタ
    * @param position 始点
    * @param direction 方向ベクトル（正規化）
    * @param length 長さ
    */
    Segment2D(const Vector2& position, const Vector2& direction, float length)
        :position(position), direction(direction), length(length) { };
    /**
    * @brief コンストラクタ
    * @param start 始点
    * @param end 終点
    */
    Segment2D(const Vector2& start, const Vector2& end)
        :Segment2D(start, (end - start).getNormal(), (end - start).length()) { }
    /**
    * @brief 終点の取得
    */
    Vector2 getEndPosition() const {
        return position + direction * length;
    };
public:
    Vector2 position; //!< 始点
    Vector2 direction; //!< 方向ベクトル
    float length; //!< 線分の長さ
};

} //Math 
} //Framework 
