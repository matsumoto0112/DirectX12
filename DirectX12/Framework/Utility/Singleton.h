#pragma once

#include <memory>
namespace Framework {
namespace Utility {

/**
* @class Singleton
* @brief 継承したクラスをシングルトン化
*/
template <class T>
class Singleton {
public:
    /**
    * @brief コンストラクタ
    */
    Singleton() {};
    /**
    : @brief デストラクタ
    */
    virtual ~Singleton() = default;
    /**
    * @brief インスタンスの取得
    * @return インスタンス
    */
    static T& getInstance() {
        struct temp : T { temp() :T() {} };
        static temp instance;
        return instance;
    }
private:
    //コピーコンストラクタ禁止
    Singleton(const Singleton& t) = delete;
    //代入禁止
    Singleton& operator=(const Singleton& t) = delete;
};

} // Utility
} //Framework 
