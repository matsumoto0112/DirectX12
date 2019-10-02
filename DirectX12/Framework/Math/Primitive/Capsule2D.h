#pragma once
#include "Framework/Math/Primitive/Segment2D.h"

namespace Framework {
namespace Math {


/**
* @class Capsule2D
* @brief �J�v�Z��
*/
class Capsule2D {
public:
    /**
    * @brief �R���X�g���N�^
    */
    Capsule2D() :Capsule2D(Segment2D(), 0.0f) { };
    /**
    * @brief �R���X�g���N�^
    * @param segment ���S����
    * @param radius ���a
    */
    Capsule2D(const Segment2D& segment, float radius)
        :segment(segment), radius(radius) { };
public:
    Segment2D segment; //!< ���S����
    float radius; //!< ���a
};

} //Math 
} //Framework 