#pragma once

#include <string>
#include <sstream>

namespace Framework {
namespace Utility {

/**
* @class StringBuilder
* @brief 文字列連結クラス
*/
class StringBuilder {
public:
    /**
    * @brief コンストラクタ
    * @tparam T "<<"を実装している任意の型
    * @param src 連結元
    */
    template <class T>
    StringBuilder(const T& src);
    /**
    * @brief デストラクタ
    */
    ~StringBuilder() = default;
    /**
    * @brief 文字列の結合
    */
    StringBuilder& operator<<(const StringBuilder& src);
    /**
    * @brief 文字列の結合
    */
    template <class T>
    StringBuilder& operator<<(const T& src);
    /**
    * @brief 他の文字列と連結
    */
    StringBuilder operator+(const StringBuilder& other);
    /**
    * @brief 文字列の取得
    */
    const std::string& getStr() const { return s; }
    /**
    * @brief キャスト
    */
    operator const char*() { return s.c_str(); }
    /**
    * @brief キャスト
    */
    operator std::string() { return s; }
private:
    std::string s; //!< 現在の文字列
};

template<class T>
inline StringBuilder::StringBuilder(const T& src) {
    std::stringstream ss;
    ss << src;
    s = ss.str();
}

inline StringBuilder& StringBuilder::operator<<(const Utility::StringBuilder& src) {
    s += src.s;
    return *this;
}

inline StringBuilder StringBuilder::operator+(const Utility::StringBuilder& other) {
    return StringBuilder(s + other.s);
}

template<class T>
inline StringBuilder& StringBuilder::operator<<(const T& src) {
    std::stringstream ss;
    ss << src;
    s += ss.str();
    return *this;
}

} //Utility 
} //Framework 