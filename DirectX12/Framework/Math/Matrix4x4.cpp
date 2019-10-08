#include "Matrix4x4.h"
#include "Framework/Math/Vector2.h"
#include "Framework/Math/Matrix3x3.h"
#include "Framework/Math/Quaternion.h"
namespace Framework {
namespace Math {

const Matrix4x4 Matrix4x4::IDENTITY = Matrix4x4(
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
);

const Matrix4x4 Matrix4x4::ZERO = Matrix4x4(
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f
);

Matrix4x4::Matrix4x4()
    :m{ {} } { }

Matrix4x4::Matrix4x4(float m11, float m12, float m13, float m14,
    float m21, float m22, float m23, float m24,
    float m31, float m32, float m33, float m34,
    float m41, float m42, float m43, float m44)
    : m({ {{m11,m12,m13,m14},{m21,m22,m23,m24},{m31,m32,m33,m34},{m41,m42,m43,m44}} }) { }

Matrix4x4::Matrix4x4(std::array<std::array<float, 4>, 4> m)
    : m(m) { }

Matrix4x4& Matrix4x4::operator=(const Matrix4x4& mat) {
    this->m = mat.m;
    return *this;
}

bool Matrix4x4::operator==(const Matrix4x4& mat) const {
    return  m == mat.m;
}

bool Matrix4x4::operator!=(const Matrix4x4& mat) const {
    return m != mat.m;
}

Matrix4x4 Matrix4x4::operator+() {
    Matrix4x4 mat(m);
    return mat;
}

Matrix4x4 Matrix4x4::operator-() {
    Matrix4x4 mat(m);
    mat *= -1;
    return mat;
}

Matrix4x4 Matrix4x4::operator+(const Matrix4x4& mat) const {
    Matrix4x4 result(m);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.m[i][j] += mat.m[i][j];
        }
    }
    return result;
}

Matrix4x4 Matrix4x4::operator-(const Matrix4x4& mat) const {
    Matrix4x4 result(m);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.m[i][j] -= mat.m[i][j];
        }
    }
    return result;
}

Matrix4x4 Matrix4x4::operator*(float k) const {
    Matrix4x4 result(m);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.m[i][j] *= k;
        }
    }
    return result;
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4& mat) const {
    Matrix4x4 result;
    result.m[0][0] = m[0][0] * mat.m[0][0] + m[0][1] * mat.m[1][0] + m[0][2] * mat.m[2][0] + m[0][3] * mat.m[3][0];
    result.m[0][1] = m[0][0] * mat.m[0][1] + m[0][1] * mat.m[1][1] + m[0][2] * mat.m[2][1] + m[0][3] * mat.m[3][1];
    result.m[0][2] = m[0][0] * mat.m[0][2] + m[0][1] * mat.m[1][2] + m[0][2] * mat.m[2][2] + m[0][3] * mat.m[3][2];
    result.m[0][3] = m[0][0] * mat.m[0][3] + m[0][1] * mat.m[1][3] + m[0][2] * mat.m[2][3] + m[0][3] * mat.m[3][3];

    result.m[1][0] = m[1][0] * mat.m[0][0] + m[1][1] * mat.m[1][0] + m[1][2] * mat.m[2][0] + m[1][3] * mat.m[3][0];
    result.m[1][1] = m[1][0] * mat.m[0][1] + m[1][1] * mat.m[1][1] + m[1][2] * mat.m[2][1] + m[1][3] * mat.m[3][1];
    result.m[1][2] = m[1][0] * mat.m[0][2] + m[1][1] * mat.m[1][2] + m[1][2] * mat.m[2][2] + m[1][3] * mat.m[3][2];
    result.m[1][3] = m[1][0] * mat.m[0][3] + m[1][1] * mat.m[1][3] + m[1][2] * mat.m[2][3] + m[1][3] * mat.m[3][3];

    result.m[2][0] = m[2][0] * mat.m[0][0] + m[2][1] * mat.m[1][0] + m[2][2] * mat.m[2][0] + m[2][3] * mat.m[3][0];
    result.m[2][1] = m[2][0] * mat.m[0][1] + m[2][1] * mat.m[1][1] + m[2][2] * mat.m[2][1] + m[2][3] * mat.m[3][1];
    result.m[2][2] = m[2][0] * mat.m[0][2] + m[2][1] * mat.m[1][2] + m[2][2] * mat.m[2][2] + m[2][3] * mat.m[3][2];
    result.m[2][3] = m[2][0] * mat.m[0][3] + m[2][1] * mat.m[1][3] + m[2][2] * mat.m[2][3] + m[2][3] * mat.m[3][3];

    result.m[3][0] = m[3][0] * mat.m[0][0] + m[3][1] * mat.m[1][0] + m[3][2] * mat.m[2][0] + m[3][3] * mat.m[3][0];
    result.m[3][1] = m[3][0] * mat.m[0][1] + m[3][1] * mat.m[1][1] + m[3][2] * mat.m[2][1] + m[3][3] * mat.m[3][1];
    result.m[3][2] = m[3][0] * mat.m[0][2] + m[3][1] * mat.m[1][2] + m[3][2] * mat.m[2][2] + m[3][3] * mat.m[3][2];
    result.m[3][3] = m[3][0] * mat.m[0][3] + m[3][1] * mat.m[1][3] + m[3][2] * mat.m[2][3] + m[3][3] * mat.m[3][3];

    return result;
}

Matrix4x4 Matrix4x4::operator/(float k) const {
    Matrix4x4 mat(m);
    float oneOverK = 1.0f / k;
    return mat * oneOverK;
}

Matrix4x4& Matrix4x4::operator+=(const Matrix4x4& mat) {
    *this = *this + mat;
    return *this;
}

Matrix4x4& Matrix4x4::operator-=(const Matrix4x4& mat) {
    *this = *this - mat;
    return *this;
}

Matrix4x4& Matrix4x4::operator*=(float k) {
    *this = *this * k;
    return *this;
}

Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& mat) {
    *this = *this * mat;
    return *this;
}

Matrix4x4& Matrix4x4::operator/=(float k) {
    *this = *this / k;
    return *this;
}

Matrix4x4 Matrix4x4::createTranslate(const Vector3& v) {
    return Matrix4x4
    (
        1.0f, 0, 0, 0,
        0, 1.0f, 0, 0,
        0, 0, 1.0f, 0,
        v.x, v.y, v.z, 1.0f
    );
}

Matrix4x4 Matrix4x4::createTranslate(float x, float y, float z) {
    return Matrix4x4
    (
        1.0f, 0, 0, 0,
        0, 1.0f, 0, 0,
        0, 0, 1.0f, 0,
        x, y, z, 1
    );
}

Matrix4x4& Matrix4x4::setupTransform(const Vector3& v) {
    *this = createTranslate(v);
    return *this;
}

Matrix4x4& Matrix4x4::setupTransform(float x, float y, float z) {
    *this = createTranslate(x, y, z);
    return *this;
}

Matrix4x4& Matrix4x4::move(const Vector3& v) {
    m[3][0] += v.x;
    m[3][1] += v.y;
    m[3][2] += v.z;
    return *this;
}

Matrix4x4& Matrix4x4::move(float x, float y, float z) {
    m[3][0] += x;
    m[3][1] += y;
    m[3][2] += z;
    return *this;
}

Matrix4x4 Matrix4x4::createRotationX(float degree) {
    const float sin = MathUtil::sin(degree);
    const float cos = MathUtil::cos(degree);
    return Matrix4x4
    (
        1.0f, 0, 0, 0,
        0, cos, sin, 0,
        0, -sin, cos, 0,
        0, 0, 0, 1
    );
}

Matrix4x4& Matrix4x4::setupRotationX(float degree) {
    *this = createRotationX(degree);
    return *this;
}

Matrix4x4 Matrix4x4::createRotationY(float degree) {
    const float sin = MathUtil::sin(degree);
    const float cos = MathUtil::cos(degree);
    return Matrix4x4
    (
        cos, 0, -sin, 0,
        0, 1, 0, 0,
        sin, 0, cos, 0,
        0, 0, 0, 1
    );
}

Matrix4x4& Matrix4x4::setupRotationY(float degree) {
    *this = createRotationY(degree);
    return *this;
}

Matrix4x4 Matrix4x4::createRotationZ(float degree) {
    const float sin = MathUtil::sin(degree);
    const float cos = MathUtil::cos(degree);
    return Matrix4x4
    (
        cos, sin, 0, 0,
        -sin, cos, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    );
}

Matrix4x4& Matrix4x4::setupRotationZ(float degree) {
    *this = createRotationZ(degree);
    return *this;
}

Matrix4x4 Matrix4x4::createRotation(const Vector3& r) {
    Matrix4x4 mx(createRotationX(r.x));
    Matrix4x4 my(createRotationY(r.y));
    Matrix4x4 mz(createRotationZ(r.z));
    return mx * my * mz;
}

Matrix4x4& Matrix4x4::setupRotation(const Vector3& r) {
    *this = createRotation(r);
    return *this;
}

Matrix4x4& Matrix4x4::rotateX(float degree) {
    Matrix4x4 r(createRotationX(degree));
    *this = *this * r;
    return *this;
}

Matrix4x4& Matrix4x4::rotateY(float degree) {
    Matrix4x4 r(createRotationY(degree));
    *this = *this * r;
    return *this;
}

Matrix4x4& Matrix4x4::rotateZ(float degree) {
    Matrix4x4 r(createRotationZ(degree));
    *this = *this * r;
    return *this;
}

Matrix4x4& Matrix4x4::rotate(const Vector3& v) {
    Matrix4x4 r(createRotation(v));
    *this = *this * r;
    return *this;
}

Matrix4x4 Matrix4x4::createScale(float s) {
    return Matrix4x4
    (
        s, 0, 0, 0,
        0, s, 0, 0,
        0, 0, s, 0,
        0, 0, 0, 1
    );
}

Matrix4x4 Matrix4x4::createScale(const Vector3& s) {
    return createScale(s.x, s.y, s.z);
}

Matrix4x4& Matrix4x4::setupScale(float s) {
    *this = createScale(s);
    return *this;
}

Matrix4x4& Matrix4x4::setupScale(const Vector3& s) {
    *this = createScale(s);
    return *this;
}

Matrix4x4 Matrix4x4::createScale(float sx, float sy, float sz) {
    return Matrix4x4
    (
        sx, 0, 0, 0,
        0, sy, 0, 0,
        0, 0, sz, 0,
        0, 0, 0, 1
    );
}

Matrix4x4& Matrix4x4::scale(float s) {
    Matrix4x4 r(createScale(s));
    *this = *this * r;
    return *this;
}

Matrix4x4& Matrix4x4::scale(const Vector3& s) {
    Matrix4x4 r(createScale(s));
    *this = *this * r;
    return *this;
}

Matrix4x4 Matrix4x4::createView(const ViewInfo& info) {
    const Vector3 zaxis = (info.at - info.eye).getNormal();
    const Vector3 xaxis = info.up.cross(zaxis).getNormal();
    const Vector3 yaxis = zaxis.cross(xaxis);
    return  Matrix4x4
    (
        xaxis.x, yaxis.x, zaxis.x, 0.0f,
        xaxis.y, yaxis.y, zaxis.y, 0.0f,
        xaxis.z, yaxis.z, zaxis.z, 0.0f,
        -xaxis.dot(info.eye), -yaxis.dot(info.eye), -zaxis.dot(info.eye), 1.0f
    );
}

Matrix4x4& Matrix4x4::setUpView(const ViewInfo& info) {
    *this = createView(info);
    return *this;
}

Matrix4x4 Matrix4x4::createProjection(const ProjectionInfo& info) {
    const float yScale = MathUtil::cot(info.fov / 2.0f);
    const float xScale = info.aspect * yScale;
    float subZ = info.farZ - info.nearZ;
    return Matrix4x4
    (
        xScale, 0, 0, 0,
        0, yScale, 0, 0,
        0, 0, info.farZ / subZ, 1.0f,
        0, 0, -info.nearZ * info.farZ / subZ, 0
    );
}

Matrix4x4& Matrix4x4::setProjection(const ProjectionInfo& info) {
    *this = createProjection(info);
    return *this;
}

Math::Vector3 Matrix4x4::getTranslate() const {
    return Math::Vector3(m[3][0], m[3][1], m[3][2]);
}

Matrix4x4 Matrix4x4::createOrthographic(const Vector2& screenSize) {
    return Matrix4x4
    (
        2 / screenSize.x, 0, 0, 0,
        0, -2 / screenSize.y, 0, 0,
        0, 0, 1.0f, 0,
        -1.0f, 1.0f, 0, 1.0f
    );
}

Matrix4x4& Matrix4x4::setUpOrthographic(const Vector2& screenSize) {
    *this = createOrthographic(screenSize);
    return *this;
}

Quaternion Matrix4x4::toQuaternion() const {
    //w,_x,_y,z‚Ì‚¤‚¿“x‚Ì’l‚ªˆê”Ô‘å‚«‚¢‚©’²¸
    const float fourWSquaredMinus1 = m[0][0] + m[1][1] + m[2][2];
    const float fourXSquaredMinus1 = m[0][0] - m[1][1] - m[2][2];
    const float fourYSquaredMinus1 = -m[0][0] + m[1][1] - m[2][2];
    const float fourZSquaredMinus1 = -m[0][0] - m[1][1] + m[2][2];

    int biggestIndex = 0;
    float fourBiggestSquaredMinus1 = fourWSquaredMinus1;
    if (fourXSquaredMinus1 > fourBiggestSquaredMinus1) {
        fourBiggestSquaredMinus1 = fourXSquaredMinus1;
        biggestIndex = 1;
    }
    if (fourYSquaredMinus1 > fourBiggestSquaredMinus1) {
        fourBiggestSquaredMinus1 = fourYSquaredMinus1;
        biggestIndex = 2;
    }
    if (fourZSquaredMinus1 > fourBiggestSquaredMinus1) {
        fourBiggestSquaredMinus1 = fourZSquaredMinus1;
        biggestIndex = 3;
    }
    const float biggestValue = sqrtf(fourBiggestSquaredMinus1 + 1.0f) * 0.5f;
    const float mult = 0.25f / biggestValue;
    float x, y, z, w;
    switch (biggestIndex) {
    case 0:
        x = (m[1][2] - m[2][1]) * mult;
        y = (m[2][0] - m[0][2]) * mult;
        z = (m[0][1] - m[1][0]) * mult;
        w = biggestValue;
        break;
    case 1:
        x = biggestValue;
        y = (m[0][1] + m[1][0]) * mult;
        z = (m[2][0] + m[0][2]) * mult;
        w = (m[1][2] - m[2][1]) * mult;
        break;
    case 2:
        x = (m[1][0] + m[0][1]) * mult;
        y = biggestValue;
        z = (m[1][2] + m[2][1]) * mult;
        w = (m[2][0] - m[0][2]) * mult;
        break;
    case 3:
        x = (m[2][0] + m[0][2]) * mult;
        y = (m[1][2] + m[2][1]) * mult;
        z = biggestValue;
        w = (m[0][1] - m[1][0]) * mult;
        break;
    }
    return Quaternion(x, y, z, w);
}

Matrix4x4 Matrix4x4::transpose() const {
    return Matrix4x4(
        m[0][0], m[1][0], m[2][0], m[3][0],
        m[0][1], m[1][1], m[2][1], m[3][1],
        m[0][2], m[1][2], m[2][2], m[3][2],
        m[0][3], m[1][3], m[2][3], m[3][3]
    );
}

Matrix4x4 Matrix4x4::transposition(const Matrix4x4& mat) {
    return Matrix4x4(
        mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0],
        mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1],
        mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2],
        mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3]
    );
}

float Matrix4x4::determinant(const Matrix4x4& mat) {
    float buf = 0.0f;
    Matrix4x4 m(mat);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (i < j) {
                buf = m.m[j][i] / m.m[i][i];
                for (int k = 0; k < 4; k++) {
                    m.m[j][k] -= m.m[i][k] * buf;
                }
            }
        }
    }
    float det = 1.0f;
    for (int i = 0; i < 4; i++) {
        det *= m.m[i][i];
    }
    return det;
}

Matrix4x4 Matrix4x4::inverse(const Matrix4x4& mat) {
    Matrix4x4 res = Matrix4x4::IDENTITY;
    Matrix4x4 m(mat);
    float buf = 0.0;
    for (int i = 0; i < 4; i++) {
        buf = 1.0f / m.m[i][i];
        for (int j = 0; j < 4; j++) {
            m.m[i][j] *= buf;
            res.m[i][j] *= buf;
        }
        for (int j = 0; j < 4; j++) {
            if (i != j) {
                buf = m.m[j][i];
                for (int k = 0; k < 4; k++) {
                    m.m[j][k] -= m.m[i][k] * buf;
                    res.m[j][k] -= res.m[i][k] * buf;
                }
            }
        }
    }
    return res;
}

Matrix3x3 Matrix4x4::toMatrix3(const Matrix4x4& mat) {
    return Matrix3x3
    (
        mat.m[0][0], mat.m[0][1], mat.m[0][2],
        mat.m[1][0], mat.m[1][1], mat.m[1][2],
        mat.m[2][0], mat.m[2][1], mat.m[2][2]
    );
}

Matrix4x4 Matrix4x4::lerp(const Matrix4x4& mat1, const Matrix4x4& mat2, float t) {
    return mat1 * (1.0f - t) + mat2 * t;
}

Vector3 Matrix4x4::multiplyCoord(const Math::Vector3& v, const Math::Matrix4x4& m) {
    Math::Matrix4x4 mat = createTranslate(v) * m;
    Math::Vector3 res(mat.m[3][0] / mat.m[3][3], mat.m[3][1] / mat.m[3][3], mat.m[3][2] / mat.m[3][3]);
    return res;
}

std::array<float, 4>& Matrix4x4::operator[](int n) {
    return m[n];
}

Vector3 operator*(const Vector3& v, const Matrix4x4& mat) {
    float x = v.x * mat.m[0][0] + v.y * mat.m[1][0] + v.z * mat.m[2][0] + mat.m[3][0];
    float y = v.x * mat.m[0][1] + v.y * mat.m[1][1] + v.z * mat.m[2][1] + mat.m[3][1];
    float z = v.x * mat.m[0][2] + v.y * mat.m[1][2] + v.z * mat.m[2][2] + mat.m[3][2];
    return Vector3(x, y, z);
}

Vector3& operator*=(Vector3& v, const Matrix4x4& mat) {
    v = v * mat;
    return v;
}

std::ostream& operator<<(std::ostream& os, const Matrix4x4& mat) {
    os << mat.m[0][0] << " " << mat.m[0][1] << " " << mat.m[0][2] << " " << mat.m[0][3] << "\n"
        << mat.m[1][0] << " " << mat.m[1][1] << " " << mat.m[1][2] << " " << mat.m[1][3] << "\n"
        << mat.m[2][0] << " " << mat.m[2][1] << " " << mat.m[2][2] << " " << mat.m[2][3] << "\n"
        << mat.m[3][0] << " " << mat.m[3][1] << " " << mat.m[3][2] << " " << mat.m[3][3] << "\n";
    return os;
}

} // Math
} //Framework 