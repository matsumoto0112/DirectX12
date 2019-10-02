#pragma once

#include <array>

namespace Framework {
namespace Graphics {
/**
* @class Color4
* @brief 色
*/
class Color4 {
public:
    float r; //!< r成分
    float g; //!< g成分
    float b; //!< b成分
    float a; //!< a成分
public:
    static const Color4 BLACK; //!< 黒
    static const Color4 WHITE; //!< 白
public:
    /**
    * @brief コンストラクタ
    */
    Color4();
    /**
    * @brief コンストラクタ
    * @param r r成分(0.0～1.0)
    * @param g g成分(0.0～1.0)
    * @param b b成分(0.0～1.0)
    * @param a a成分(0.0～1.0)
    */
    Color4(float r, float g, float b, float a);
    /**
    * @brief コンストラクタ
    * @param color 色配列
    */
    Color4(float color[4]);
    /**
    * @brief コピーコンストラクタ
    */
    Color4(const Color4& c);
    /**
    * @brief デストラクタ
    */
    ~Color4();
    /**
    * @brief 代入演算子
    */
    Color4& operator=(const Color4& c);
    /**
    * @brief 等値演算子
    */
    bool operator==(const Color4& c);
    /**
    * @brief 等値演算子
    */
    bool operator!=(const Color4& c);
    /**
    * v単項プラス演算子
    */
    Color4 operator+();
    /**
    * @brief 単項マイナス演算子
    */
    Color4 operator-();
    /**
    * @brief 配列の取得
    * @param c 戻り値
    */
    void get(float c[4]) const;
    /**
    * @brief 色を配列で取得
    * @return r,g,b,aの順に格納された配列
    */
    std::array<float, 4> get() const;
    /**
    * @brief 補間
    * @param a 色1
    * @param b 色2
    * @param t 補間係数
    */
    static Color4 lerp(const Color4& a, const Color4& b, float t);
};

} //Graphics 
} //Framework 
