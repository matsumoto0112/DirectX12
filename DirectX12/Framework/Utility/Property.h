#pragma once
#include <functional>

namespace Framework {
namespace Utility {

/**
* @brief プロパティサポートクラス
*/
template<class T>
class Property {
public:
    T& r;
    std::function<void(T value)> set = nullptr;
    std::function<T()> get = nullptr;
public:
    operator T() {
        return get ? this->get() : this->r;
    }

    T operator ->() {
        return get ? this->get() : this->r;
    }

    void operator =(const T v) {
        if (set) {
            this->set(v);
        }
        else {
            r = v;
        }
    }
};

/**
* @brief ゲッターのサポートクラス
*/
template <class T>
class GetterProperty {
public:
    T& r;
    std::function<T()> get = nullptr;
public:
    operator T() {
        return get ? this->get() : this->r;
    }

    T operator ->() {
        return get ? this->get() : this->r;
    }
};

template <class T>
class GetRefProperty {
public:
    T& r;
    std::function<T & ()> get = nullptr;
public:
    operator T& () {
        return get ? this->get() : this->r;
    }

    T& operator ->() {
        return get ? this->get() : this->r;
    }
};

} //Utility 
} //Framework 