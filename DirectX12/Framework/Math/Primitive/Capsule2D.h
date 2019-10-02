#pragma once
#include "Framework/Math/Primitive/Segment2D.h"

namespace Framework {
namespace Math {


/**
* @class Capsule2D
* @brief カプセル
*/
class Capsule2D {
public:
    /**
    * @brief コンストラクタ
    */
    Capsule2D() :Capsule2D(Segment2D(), 0.0f) { };
    /**
    * @brief コンストラクタ
    * @param segment 中心線分
    * @param radius 半径
    */
    Capsule2D(const Segment2D& segment, float radius)
        :segment(segment), radius(radius) { };
public:
    Segment2D segment; //!< 中心線分
    float radius; //!< 半径
};

} //Math 
} //Framework 