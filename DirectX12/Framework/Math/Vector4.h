#pragma once

#include <ostream>
#include "Framework/Math/MathUtility.h"
namespace Framework {
namespace Math {

/**
* @class Vector4
* @brief 4�����x�N�g��
*/
class Vector4 {
public:
    float x; //!< x����
    float y; //!< y����
    float z; //!< z����
    float w; //!< w����
public:
    static const Vector4 ZERO; //!< (0,0,0,0)
    /**
    * @brief �R���X�g���N�^
    */
    Vector4(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 0.0f);
    /**
    * @brief �R�s�[�R���X�g���N�^
    */
    Vector4(const Vector4& v);
    /**
    * @brief ������Z�q
    */
    Vector4& operator =(const Vector4& a);
    /**
    * @brief ���l���Z�q
    */
    bool operator ==(const Vector4& a) const;
    /**
    * @brief ���l���Z�q
    */
    bool operator !=(const Vector4& a) const;
    /**
    * @brief �P���v���X���Z�q
    */
    Vector4 operator +() const;
    /**
    * @brief �P���}�C�i�X���Z�q
    */
    Vector4 operator -() const;
    /**
    * @brief ���Z
    */
    Vector4 operator +(const Vector4& a) const;
    /**
    * @brief ���Z
    */
    Vector4 operator -(const Vector4& a) const;
    /**
    * @brief �X�J���[�{
    */
    Vector4 operator *(float a) const;
    /**
    * @brief �X�J���[���Z
    */
    Vector4 operator /(float a) const;
    /**
    * @brief �X�J���[�{
    */
    friend Vector4 operator *(float a, const Vector4& v);
    /**
    * @brief �X�J���[���Z
    */
    friend Vector4 operator /(float a, const Vector4& v);
    /**
    * @brief ���Z������Z�q
    */
    Vector4& operator +=(const Vector4& a);
    /**
    * @brief ���Z������Z�q
    */
    Vector4& operator -=(const Vector4& a);
    /**
    * @brief ��Z������Z�q
    */
    Vector4& operator *=(float a);
    /**
    * @brief ���Z������Z�q
    */
    Vector4& operator /=(float a);
    /**
    * @brief �傫����2���Ԃ�
    * @return �x�N�g���̑傫����2��
    */
    float lengthSq() const;
    /**
    * @brief �傫����Ԃ�
    * @return �x�N�g���̑傫��
    */
    float length() const;
    /**
    * @brief �x�N�g���𐳋K������
    */
    void normalize();
    /**
    * @brief �x�N�g���̐��K��
    * @return ���K�����ꂽ�x�N�g����Ԃ�
    */
    Vector4 getNormal() const;
    /**
    * @brief ����
    * @param a �v�Z����x�N�g��
    * @return ����
    */
    float dot(const Vector4& a) const;
    /**
    * @brief ����
    * @param v1 �x�N�g��1
    * @param v2 �x�N�g��2
    */
    static float dot(const Vector4& v1, const Vector4& v2);
    /**
    * @brief �x�N�g���̃N�����v����
    * @param min ����
    * @param max ���
    */
    void clamp(const Vector4& min, const Vector4& max);
    /**
    * @brief �o�͉��Z�q
    */
    friend std::ostream& operator<<(std::ostream& oss, const Math::Vector4& v);
};

} //Math
} //Framework 