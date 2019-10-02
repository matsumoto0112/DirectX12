#include "Quaternion.h"
#include <math.h>
#include "Vector3.h"
#include "Matrix4x4.h"
#include "Framework/Utility/Debug.h"
#include "MathUtility.h"

namespace Framework {
namespace Math {
const Quaternion Quaternion::IDENTITY = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);

Quaternion::Quaternion() {
    identity();
}

Quaternion::Quaternion(float x, float y, float z, float w)
    :x(x), y(y), z(z), w(w) {}

void Quaternion::identity() {
    *this = IDENTITY;
}

void Quaternion::setToRotateAboutX(float degree) {
    const float thetaOver2 = MathUtil::toRadian(degree) * 0.5f;
    w = cosf(thetaOver2);
    x = sinf(thetaOver2);
    y = 0.0f;
    z = 0.0f;
}

void Quaternion::setToRotateAboutY(float degree) {
    const float thetaOver2 = degree * 0.5f;
    w = MathUtil::cos(thetaOver2);
    x = 0.0f;
    y = MathUtil::sin(thetaOver2);
    z = 0.0f;
}

void Quaternion::setToRotateAboutZ(float degree) {
    const float thetaOver2 = MathUtil::toRadian(degree) * 0.5f;
    w = cosf(thetaOver2);
    x = 0.0f;
    y = 0.0f;
    z = sinf(thetaOver2);
}
void Quaternion::setToRotateAboutAxis(const Vector3& axis, float degree) {
    MY_ASSERTION(
        fabs(axis.length() - 1.0f) < EPSILON,
        "‰ñ“]Ž²‚ª³‹K‰»‚³‚ê‚Ä‚¢‚Ü‚¹‚ñB");
    const float thetaOver2 = MathUtil::toRadian(degree) * 0.5f;
    const float sinThetaOver2 = sinf(thetaOver2);
    w = cosf(thetaOver2);
    x = sinThetaOver2 * axis.x;
    y = sinThetaOver2 * axis.y;
    z = sinThetaOver2 * axis.z;
}

Quaternion Quaternion::createRotateAboutX(float degree) {
    Quaternion res;
    res.setToRotateAboutX(degree);
    return res;
}

Quaternion Quaternion::createRotateAboutY(float degree) {
    Quaternion res;
    res.setToRotateAboutY(degree);
    return res;
}

Quaternion Quaternion::createRotateAboutZ(float degree) {
    Quaternion res;
    res.setToRotateAboutZ(degree);
    return res;
}

Quaternion Quaternion::createRotateAboutAxis(const Vector3& axis, float degree) {
    Quaternion res;
    res.setToRotateAboutAxis(axis, degree);
    return res;
}

Quaternion Quaternion::operator*(const Quaternion& a) const {
    Quaternion result;
    result.w = w * a.w - (x * a.x + y * a.y + z * a.z);
    result.x = w * a.x + x * a.w + z * a.y - y * a.z;
    result.y = w * a.y + y * a.w + x * a.z - z * a.x;
    result.z = w * a.z + z * a.w + y * a.x - x * a.y;
    return result;
}

Matrix4x4 Quaternion::toMatrix() const {
    const float twoXX = 2.0f * x * x;
    const float twoYY = 2.0f * y * y;
    const float twoZZ = 2.0f * z * z;
    const float twoXY = 2.0f * x * y;
    const float twoYZ = 2.0f * y * z;
    const float twoZX = 2.0f * z * x;
    const float twoWX = 2.0f * w * x;
    const float twoWY = 2.0f * w * y;
    const float twoWZ = 2.0f * w * z;
    return Matrix4x4
    (
        1.0f - twoYY - twoZZ,   twoXY + twoWZ,          twoZX - twoWY,          0,
        twoXY - twoWZ,          1.0f - twoXX - twoZZ,   twoYZ + twoWX,          0,
        twoZX + twoWY,          twoYZ - twoWX,          1.0f - twoXX - twoYY,   0,
        0, 0, 0, 1
    );
}

float Quaternion::dot(const Quaternion& a) const {
    return w * a.w + x * a.x + y * a.y + z * a.z;
}

void Quaternion::normalize() {
    const float len = MathUtil::sqrt(w * w + x * x + y * y + z * z);
    if (len > 0) {
        float oneOverLen = 1.0f / len;
        w *= oneOverLen;
        x *= oneOverLen;
        y *= oneOverLen;
        z *= oneOverLen;
    }
}

Vector3 Quaternion::toEular() const {
    float rx, ry, rz;
    float sy = -2.0f *(y * z + w * x);
    if (MathUtil::abs(sy) > 0.9999f) {
        rx = PI / 2 * sy;
        ry = MathUtil::atan2(-x * z - w * y, 0.5f - y * y - z * z);
        rz = 0.0f;
    }
    else {
        rx = MathUtil::asin(sy);
        ry = MathUtil::atan2(x * z - w * y, 0.5f - x * x - y * y);
        rz = MathUtil::atan2(x * y - w * z, 0.5f - x * x - z * z);
    }
    return Vector3(rx, ry, rz);
}

Quaternion Quaternion::fromEular(const Math::Vector3& eular) {
    float x = MathUtil::toRadian(eular.x) * 0.5f;
    float y = MathUtil::toRadian(eular.y) * 0.5f;
    float z = MathUtil::toRadian(eular.z) * 0.5f;

    float c1 = std::cosf(x);
    float c2 = std::cosf(y);
    float c3 = std::cosf(z);

    float s1 = std::sinf(x);
    float s2 = std::sinf(y);
    float s3 = std::sinf(z);

    Quaternion res;
    res.x = s1 * c2 * c3 + c1 * s2 * s3;
    res.y = c1 * s2 * c3 - s1 * c2 * s3;
    res.z = c1 * c2 * s3 - s1 * s2 * c3;
    res.w = c1 * c2 * c3 + s1 * s2 * s3;

    return res;
}

float Quaternion::getRotatAngle() const {
    float thetaOver2 = std::acosf(w);
    return thetaOver2 * 2.0f;
}

Vector3 Quaternion::getRotateAxis() const {
    float sinThetaOver2Sq = 1.0f - w * w;
    if (sinThetaOver2Sq <= 0.0f) {
        return Vector3::RIGHT;
    }

    float oneOverSinThetaOver2 = 1.0f / MathUtil::sqrt(sinThetaOver2Sq);
    return Vector3(
        x * oneOverSinThetaOver2,
        y * oneOverSinThetaOver2,
        z * oneOverSinThetaOver2
    );
}

Math::Quaternion Quaternion::conjugate(const Math::Quaternion& q) {
    return Math::Quaternion(-q.x, -q.y, -q.z, q.w);
}

Math::Quaternion Quaternion::createLookTarget(const Math::Vector3& v, const Math::Vector3& normal) {
    MY_ASSERTION((v.length() - 1 <= EPSILON), "•ûŒüƒxƒNƒgƒ‹‚ð³‹K‰»‚µ‚Ä‚­‚¾‚³‚¢");
    Math::Vector3 n = Vector3::cross(normal, v);
    n.normalize();
    float angle = MathUtil::acos(Vector3::dot(v, normal));
    Math::Quaternion q;
    q.setToRotateAboutAxis(n, MathUtil::toRadian(angle));
    return q;
}

Math::Vector3 Quaternion::multiply(const Math::Vector3& v) {
    Math::Quaternion conj = Math::Quaternion::conjugate(*this);
    Math::Quaternion q(v.x, v.y, v.z, 0.0f);
    Math::Quaternion res = conj * q * *this;
    return Math::Vector3(res.x, res.y, res.z);
}

std::ostream & operator<<(std::ostream& os, const Quaternion& q) {
    os << "(" << q.x << "," << q.y << "," << q.z << "," << q.w << ")";
    return os;
}

} //Math
} //Framework 