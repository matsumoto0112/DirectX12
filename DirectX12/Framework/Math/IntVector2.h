#pragma once
namespace Framework {
namespace Math {

/**
* @class IntVector2
* @brief 整数型Vector2
*/
class IntVector2 {
public:
    int x; //!< x成分
    int y; //!< y成分
public:
    /**
    * @brief コンストラクタ
    */
    IntVector2(int x = 0, int y = 0);
    /**
    * @brief コピーコンストラクタ
    */
    IntVector2(const IntVector2& i);
    /**
    * @brief デストラクタ
    */
    ~IntVector2();

    /**
    * @brief 代入演算子
    */
    IntVector2& operator=(const IntVector2& i);

    /**
    * @brief 等値演算子
    */
    bool operator==(const IntVector2& i)const;
    /**
    * @brief 等値演算子
    */
    bool operator !=(const IntVector2& i) const;
    /**
    * @brief 0ベクトルの取得
    */
    static IntVector2 zero();
    /**
    * @brief 単項プラス演算子
    */
    IntVector2 operator +() const;
    /**
    * @brief 単項マイナス演算子
    */
    IntVector2 operator -() const;
    /**
    * @brief 加算
    */
    IntVector2 operator +(const IntVector2& i) const;
    /**
    * @brief 減算
    */
    IntVector2 operator -(const IntVector2& i) const;
    /**
    * @brief スカラー倍
    */
    IntVector2 operator *(float s) const;
    /**
    * @brief スカラー倍
    */
    friend IntVector2 operator *(float s, const IntVector2& i);
    /**
    * @brief スカラー除算
    */
    IntVector2 operator /(float s) const;
    /**
    * @brief スカラー除算
    */
    friend IntVector2 operator /(float s, const IntVector2& i);
    /**
    * @brief 加算代入演算子
    */
    IntVector2& operator +=(const IntVector2& v);
    /**
    * @brief 減算代入演算子
    */
    IntVector2& operator -=(const IntVector2& v);
    /**
    * @brief 乗算代入演算子
    */
    IntVector2& operator *=(float s);
    /**
    * @brief 除算代入演算子
    */
    IntVector2& operator /=(float s);
};

} //Math 
} //Framework 