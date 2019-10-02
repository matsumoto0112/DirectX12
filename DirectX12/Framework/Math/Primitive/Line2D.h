#pragma once
#include "Framework/Math/Vector2.h"

namespace Framework {
namespace Math {
/**
* @class Line2D
* @brief 直線
*/
class Line2D {
public:
    /**
    * @brief コンストラクタ
    */
    Line2D() :Line2D(Vector2::ZERO, Vector2::ZERO) { }

    /**
    * @brief コンストラクタ
    * @param position 始点
    * @param direction 方向ベクトル
    */
    Line2D(const Vector2& position, const Vector2& direction)
        :position(position), direction(direction) { };
    /**
    * @brief デストラクタ
    */
    ~Line2D() { };

    /**
    * @brief 正規化された方向ベクトルを取得
    */
    inline Vector2 getNormalDirection() const {
        return direction.getNormal();
    }
public:
    Vector2 position; //!< 始点
    Vector2 direction; //!< 方向ベクトル
};

} //Math 
} //Framework 
