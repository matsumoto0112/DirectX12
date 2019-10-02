#include "IntVector2.h"

namespace Framework {
namespace Math {

IntVector2::IntVector2(int x, int y)
    :x(x), y(y) {}

IntVector2::IntVector2(const IntVector2& i)
    : IntVector2(i.x, i.y) {}

IntVector2::~IntVector2() {}

IntVector2& IntVector2::operator=(const IntVector2& i) {
    x = i.x; y = i.y;
    return *this;
}
bool IntVector2::operator==(const IntVector2& i) const {
    return (x == i.x && y == i.y);
}
bool IntVector2::operator!=(const IntVector2& i) const {
    return (x != i.x || y != i.y);
}
IntVector2 IntVector2::zero() {
    return IntVector2(0, 0);
}
IntVector2 IntVector2::operator+() const {
    return *this;
}

IntVector2 IntVector2::operator-() const {
    return IntVector2(-x, -y);
}

IntVector2 IntVector2::operator+(const IntVector2& i) const {
    return IntVector2(x + i.x, y + i.y);
}
IntVector2 IntVector2::operator-(const IntVector2& i) const {
    return IntVector2(x - i.x, y - i.y);
}
IntVector2 IntVector2::operator*(float s) const {
    return IntVector2((int)(x * s), (int)(y * s));
}
IntVector2 IntVector2::operator/(float s) const {
    const float oneOverS = 1.0f / s;
    return IntVector2((int)(x * oneOverS), (int)(y * oneOverS));
}
IntVector2& IntVector2::operator+=(const IntVector2& i) {
    x += i.x;
    y += i.y;
    return *this;
}
IntVector2& IntVector2::operator-=(const IntVector2& i) {
    x -= i.x;
    y -= i.y;
    return *this;
}
IntVector2& IntVector2::operator*=(float s) {
    x = (int)(x * s);
    y = (int)(y * s);
    return *this;
}
IntVector2& IntVector2::operator/=(float s) {
    const float oneOverS = 1.0f / s;
    x = (int)(x * oneOverS);
    y = (int)(y * oneOverS);
    return *this;
}
IntVector2 operator*(float s, const IntVector2& i) {
    return i * s;
}
IntVector2 operator/(float s, const IntVector2& i) {
    return i / s;
}

} //Math 
} //Framework 
