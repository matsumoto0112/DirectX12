#include "MathUtility.h"
#include <cmath>
#include <algorithm>

namespace Framework {
namespace Math {

float MathUtil::sin(float degree) {
    return sinf(toRadian(degree));
}

float MathUtil::cos(float degree) {
    return cosf(toRadian(degree));
}

float MathUtil::tan(float degree) {
    return tanf(toRadian(degree));
}

float MathUtil::atan2(float y, float x) {
    return toDegree(std::atan2f(y, x));
}

float MathUtil::asin(float x) {
    return toDegree(std::asin(x));
}

float MathUtil::acos(float x) {
    return toDegree(std::acos(x));
}

float MathUtil::sqrt(float a) {
    return sqrtf(a);
}

float MathUtil::cot(float degree) {
    return 1.0f / tanf(toRadian(degree));
}

float MathUtil::toRadian(float deg) {
    return deg / 180.0f * PI;
}

float MathUtil::toDegree(float rad) {
    return rad / PI * 180.0f;
}

float MathUtil::abs(float X) {
    return fabsf(X);
}
} //Math 
} //Framework 
