#include "Vector3.h"
#include "Framework/Math/MathUtility.h"
namespace Framework {
namespace Math {

const Vector3 Vector3::ZERO = Vector3(0.0f, 0.0f, 0.0f);
const Vector3 Vector3::RIGHT = Vector3(1.0f, 0.0f, 0.0f);
const Vector3 Vector3::LEFT = Vector3(-1.0f, 0.0f, 0.0f);
const Vector3 Vector3::UP = Vector3(0.0f, 1.0f, 0.0f);
const Vector3 Vector3::DOWN = Vector3(0.0f, -1.0f, 0.0f);
const Vector3 Vector3::FORWORD = Vector3(0.0f, 0.0f, 1.0f);
const Vector3 Vector3::BACK = Vector3(0.0f, 0.0f, -1.0f);

Vector3::Vector3(float x, float y, float z)
    :x(x), y(y), z(z) {}

Vector3::Vector3(const Vector3& v)
    : x(v.x), y(v.y), z(v.z) {}

Vector3& Vector3::operator=(const Vector3& a) {
    x = a.x; y = a.y; z = a.z;
    return *this;
}

bool Vector3::operator==(const Vector3 & a) const {
    return x == a.x && y == a.y && z == a.z;
}

bool Vector3::operator!=(const Vector3 & a) const {
    return x != a.x || y != a.y || z != a.z;
}

Vector3 Vector3::operator+() const {
    return Vector3(x, y, z);
}

Vector3 Vector3::operator-() const {
    return Vector3(-x, -y, -z);
}

Vector3 Vector3::operator+(const Vector3 & a) const {
    return Vector3(x + a.x, y + a.y, z + a.z);
}

Vector3 Vector3::operator-(const Vector3& a) const {
    return Vector3(x - a.x, y - a.y, z - a.z);
}

Vector3 Vector3::operator*(float a) const {
    return Vector3(x * a, y * a, z * a);
}

Vector3 Vector3::operator/(float a) const {
    float oneOverA = 1.0f / a;
    return Vector3(x * oneOverA, y * oneOverA, z * oneOverA);
}

Vector3& Vector3::operator+=(const Vector3& a) {
    x += a.x; y += a.y; z += a.z;
    return *this;
}

Vector3& Vector3::operator-=(const Vector3& a) {
    x -= a.x; y -= a.y; z -= a.z;
    return *this;
}

Vector3& Vector3::operator*=(float a) {
    x *= a; y *= a; z *= a;
    return *this;
}

Vector3& Vector3::operator/=(float a) {
    float oneOverA = 1.0f / a;
    *this *= oneOverA;
    return *this;
}

float Vector3::lengthSq() const {
    return x * x + y * y + z * z;
}

float Vector3::length() const {
    return MathUtil::sqrt(lengthSq());
}

void Vector3::normalize() {
    const float lenSq = lengthSq();
    if (lenSq > 0.0f) {
        float oneOverLenSq = 1.0f / MathUtil::sqrt(lenSq);
        x *= oneOverLenSq; y *= oneOverLenSq; z *= oneOverLenSq;
    }
}

Vector3 Vector3::getNormal() const {
    float lenSq = lengthSq();
    if (lenSq > 0.0f) {
        float oneOverLenSq = 1.0f / MathUtil::sqrt(lenSq);
        return Vector3(x * oneOverLenSq, y * oneOverLenSq, z * oneOverLenSq);
    }
    return Vector3(0.0f, 0.0f, 0.0f);
}

float Vector3::dot(const Vector3& a) const {
    return x * a.x + y * a.y + z * a.z;
}

Vector3 Vector3::cross(const Vector3& a) const {
    return Vector3(y * a.z - z * a.y, z * a.x - x * a.z, x * a.y - y * a.x);
}

void Vector3::clamp(const Vector3& min, const Vector3& max) {
    x = MathUtil::clamp<float>(x, min.x, max.x);
    y = MathUtil::clamp<float>(y, min.y, max.y);
    z = MathUtil::clamp<float>(z, min.z, max.z);
}

float Vector3::dot(const Vector3& a, const Vector3& b) {
    return a.dot(b);
}

Vector3 Vector3::cross(const Vector3& a, const Vector3& b) {
    return a.cross(b);
}

Vector3 Vector3::multiplyEashElement(const Vector3& v1, const Vector3& v2) {
    return Math::Vector3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

Vector3 operator*(float a, const Vector3& v) {
    return v * a;
}

Vector3 operator/(float a, const Vector3& v) {
    return v / a;
}

std::ostream& operator<<(std::ostream& os, const Math::Vector3& v) {
    os << "(" << v.x << "," << v.y << "," << v.z << ")";
    return os;
}

} //Math
} //Framework 