#pragma once

#include <ostream>
#include "Framework/Math/MathUtility.h"
namespace Framework {
namespace Math {

class Vector3;

/**
* @class Vector2
* @brief Vector2�N���X
*/
class Vector2 {
public:
    //! _x
    float x;
    //! _y
    float y;
public:
    static const Math::Vector2 ZERO; //!< (0,0)
    static const Math::Vector2 LEFT; //!< (-1,0)
    static const Math::Vector2 RIGHT; //!< (1,0)
    static const Math::Vector2 UP; //!< (0,1)
    static const Math::Vector2 DOWN; //!< (0,-1)
public:
    /**
    * @brief �R���X�g���N�^
    */
    Vector2(float x = 0.0f, float y = 0.0f);
    /**
    * @brief �R�s�[�R���X�g���N�^
    */
    Vector2(const Vector2& v);

    /**
    * @brief ������Z�q
    */
    Vector2& operator=(const Vector2& v);

    /**
    * @brief ���l���Z�q
    */
    bool operator==(const Vector2& v)const;
    /**
    * @brief ���l���Z�q
    */
    bool operator !=(const Vector2& v) const;
    /**
    * @brief �P���v���X���Z�q
    */
    Vector2 operator +() const;
    /**
    * @brief �P���}�C�i�X���Z�q
    */
    Vector2 operator -() const;
    /**
    * @brief ���Z
    */
    Vector2 operator +(const Vector2& v) const;
    /**
    * @brief ���Z
    */
    Vector2 operator -(const Vector2& v) const;
    /**
    * @brief �X�J���[�{
    */
    Vector2 operator *(float s) const;
    /**
    * @brief �X�J���[���Z
    */
    Vector2 operator /(float s) const;
    /**
    * @brief �X�J���[���Z
    */
    friend Vector2 operator /(float s, const Vector2& v);
    /**
    * @brief ���Z������Z�q
    */
    Vector2& operator +=(const Vector2& v);
    /**
    * @brief ���Z������Z�q
    */
    Vector2& operator -=(const Vector2& v);
    /**
    * @brief ��Z������Z�q
    */
    Vector2& operator *=(float s);
    /**
    * @brief ���Z������Z�q
    */
    Vector2& operator /=(float s);
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
    * @brief ���K�����ꂽ�x�N�g�����擾
    */
    Vector2 getNormal() const;
    /**
    * @brief ����
    * @param v �v�Z����x�N�g��
    * @return ����
    */
    float dot(const Vector2& v) const;
    /**
    * @brief ����
    * @param v1 �x�N�g��1
    * @param v2 �x�N�g��2
    */
    static float dot(const Vector2& v1, const Vector2& v2);
    /**
    * @brief �O��
    * @param v �v�Z����x�N�g��
    * @return �O��
    */
    float cross(const Vector2& v) const;
    /**
    * @brief �O��
    * @param v1 �x�N�g��1
    * @param v2 �x�N�g��2
    */
    static float cross(const Vector2& v1, const Vector2& v2);
    /**
    * @brief �x�N�g���̕��
    * @param v1 �n�_
    * @param v2 �I�_
    * @param t ��ԌW��
    * @return ��Ԍ�̃x�N�g��
    */
    static Vector2 lerp(const Vector2& v1, const Vector2& v2, float t);
    /**
    * @brief �x�N�g���̃N�����v����
    * @param min ����
    * @param max ���
    */
    void clamp(const Vector2& min, const Vector2& max);
    /**
    * @brief ���˃x�N�g�������߂�
    * @param vector ���݂̃x�N�g��
    * @param normal �@���x�N�g��
    * @return ���ˌ�̃x�N�g��
    */
    static Vector2 reflect(const Vector2& vector, const Vector2& normal);   
    /**
    * @brief Vector3�ɕϊ�����
    */
    Vector3 toVector3() const;
    /**
    * @brief �X�J���[�{
    */
    friend Vector2 operator *(float s, const Vector2& v);
    /**
    * @brief �o�͉��Z�q
    */
    friend std::ostream& operator<<(std::ostream& os, const Vector2& v);
};

} //Math
} //Framework 