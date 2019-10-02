#include "Vector4.h"
#include "Framework/Math/MathUtility.h"
namespace Framework {
namespace Math {

const Vector4 Vector4::ZERO = Vector4(0.0f, 0.0f, 0.0f, 0.0f);

Vector4::Vector4(float x, float y, float z, float w)
    :x(x), y(y), z(z), w(w) { }

Vector4::Vector4(const Vector4& v)
    : x(v.x), y(v.y), z(v.z), w(v.w) { }

Vector4& Vector4::operator=(const Vector4& a) {
    x = a.x; y = a.y; z = a.z; w = a.w;
    return *this;
}

bool Vector4::operator==(const Vector4& a) const {
    return x == a.x && y == a.y && z == a.z && w == a.w;
}

bool Vector4::operator!=(const Vector4& a) const {
    return x != a.x || y != a.y || z != a.z || w != a.w;
}

Vector4 Vector4::operator+() const {
    return Vector4(x, y, z, w);
}

Vector4 Vector4::operator-() const {
    return Vector4(-x, -y, -z, -w);
}

Vector4 Vector4::operator+(const Vector4& a) const {
    return Vector4(x + a.x, y + a.y, z + a.z, w + a.w);
}

Vector4 Vector4::operator-(const Vector4& a) const {
    return Vector4(x - a.x, y - a.y, z - a.z, w - a.w);
}

Vector4 Vector4::operator*(float a) const {
    return Vector4(x * a, y * a, z * a, w * a);
}

Vector4 Vector4::operator/(float a) const {
    float oneOverA = 1.0f / a;
    return Vector4(x * oneOverA, y * oneOverA, z * oneOverA, w * oneOverA);
}

Vector4& Vector4::operator+=(const Vector4& a) {
    x += a.x; y += a.y; z += a.z; w += a.w;
    return *this;
}

Vector4& Vector4::operator-=(const Vector4& a) {
    x -= a.x; y -= a.y; z -= a.z; w -= a.w;
    return *this;
}

Vector4& Vector4::operator*=(float a) {
    x *= a; y *= a; z *= a; w *= a;
    return *this;
}

Vector4& Vector4::operator/=(float a) {
    float oneOverA = 1.0f / a;
    *this *= oneOverA;
    return *this;
}

float Vector4::lengthSq() const {
    return x * x + y * y + z * z + w * w;
}

float Vector4::length() const {
    return MathUtil::sqrt(lengthSq());
}

void Vector4::normalize() {
    const float lenSq = lengthSq();
    if (lenSq > 0.0f) {
        float oneOverLenSq = 1.0f / MathUtil::sqrt(lenSq);
        x *= oneOverLenSq; y *= oneOverLenSq; z *= oneOverLenSq; w *= oneOverLenSq;
    }
}

Vector4 Vector4::getNormal() const {
    float lenSq = lengthSq();
    if (lenSq > 0.0f) {
        float oneOverLenSq = 1.0f / MathUtil::sqrt(lenSq);
        return Vector4(x * oneOverLenSq, y * oneOverLenSq, z * oneOverLenSq, w * oneOverLenSq);
    }
    return Vector4(0.0f, 0.0f, 0.0f, 0.0f);
}

float Vector4::dot(const Vector4& a) const {
    return x * a.x + y * a.y + z * a.z + w * a.w;
}

float Vector4::dot(const Vector4& v1, const Vector4& v2) {
    return v1.dot(v2);
}

void Vector4::clamp(const Vector4& min, const Vector4& max) {
    x = MathUtil::clamp(x, min.x, max.x);
    y = MathUtil::clamp(y, min.y, max.y);
    z = MathUtil::clamp(z, min.z, max.z);
    w = MathUtil::clamp(w, min.w, max.w);
}

Vector4 operator*(float a, const Vector4& v) {
    return v * a;
}

Vector4 operator/(float a, const Vector4& v) {
    return v / a;
}

std::ostream& operator<<(std::ostream& oss, const Math::Vector4& v) {
    oss << "(" << v.x << "," << v.y << "," << v.z << "," << v.w << ")";
    return oss;
}

} //Math
} //Framework 