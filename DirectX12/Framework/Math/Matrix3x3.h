#pragma once

#include <array>
#include "Framework/Math/Vector2.h"

namespace Framework {
namespace Math {
class Matrix4x4;

/**
* @class Matrix3x3
* @brief 3*3行列
*/
class Matrix3x3 {
public:
    std::array<std::array<float, 3>, 3> m; //!< 3x3行列
public:
    static const Matrix3x3 IDENTITY; //!< 単位行列
    static const Matrix3x3 ZERO; //!< ゼロ行列
public:
    /**
    * @brief コンストラクタ
    */
    Matrix3x3();
    /**
    * @brief コンストラクタ
    */
    Matrix3x3(float m11, float m12, float m13,
        float m21, float m22, float m23,
        float m31, float m32, float m33);
    /**
    * @brief コンストラクタ
    */
    Matrix3x3(const std::array<std::array<float, 3>, 3>& m);
    /**
    * @brief 代入演算子
    */
    Matrix3x3& operator=(const Matrix3x3& mat);
    /**
    * @brief 等値演算子
    */
    bool operator==(const Matrix3x3& mat) const;
    /**
    * @brief 等値演算子
    */
    bool operator!=(const Matrix3x3& mat) const;
    /**
    * @brief 単項プラス演算子
    */
    Matrix3x3 operator+();
    /**
    * @brief 単項マイナス演算子
    */
    Matrix3x3 operator-();
    /**
    * @brief 加算
    */
    Matrix3x3 operator+(const Matrix3x3& mat);
    /**
    * @brief 減算
    */
    Matrix3x3 operator-(const Matrix3x3& mat);
    /**
    * @brief スカラー倍
    */
    Matrix3x3 operator*(float k);
    /**
    * @brief 乗算
    */
    Matrix3x3 operator*(const Matrix3x3& mat);
    /**
    * @brief スカラー除算
    */
    Matrix3x3 operator/(float k);
    /**
    * @brief 加算代入演算子
    */
    Matrix3x3& operator+=(const Matrix3x3& mat);
    /**
    * @brief 減算代入演算子
    */
    Matrix3x3& operator-=(const Matrix3x3& mat);
    /**
    * @brief 乗算代入演算子
    */
    Matrix3x3& operator*=(float k);
    /**
    * @brief 乗算代入演算子
    */
    Matrix3x3& operator*=(const Matrix3x3& mat);
    /**
    * @brief 除算代入演算子
    */
    Matrix3x3& operator/=(float k);
    /**
    * @brief 平行移動行列の作成
    * @param v 移動量
    */
    static Matrix3x3 createTranslate(const Vector2& v);
    /**
    * @brief 回転行列の作成
    * @param degree 回転量(度)
    */
    static Matrix3x3 createRotate(float degree);
    /**
    * @brief 拡大・縮小行列の作成
    * @param s 各軸の拡大・縮小の大きさ
    */
    static Matrix3x3 createScale(const Vector2& s);
    /**
    * @brief 4*4行列への変換
    * @details 平行移動成分はなし
    */
    static Matrix4x4 toMatrix4(const Matrix3x3& mat);

    /**
    * @brief ベクトル×行列
    */
    friend Vector2 operator*(const Vector2& v, const Matrix3x3& mat);
    /**
    * @brief ベクトル×行列
    */
    friend Vector2& operator*=(Vector2& v, const Matrix3x3& mat);
    /**
    * @brief 添え字演算子 
    */
    std::array<float, 3>& operator[](int n);

};
} //Math
} //Framework 