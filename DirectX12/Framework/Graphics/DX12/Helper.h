#pragma once
#include <stdexcept>
#include <d3d12.h>

namespace Framework {
namespace Graphics {

/**
* @brief エラーを文字列に変換する
*/
inline std::string HrToString(HRESULT hr) {
    char s_str[64] = {};
    sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
    return std::string(s_str);
}

/**
* @brief HRESULT例外クラス
*/
class HrException : public std::runtime_error {
public:
    HrException(HRESULT hr) : std::runtime_error(HrToString(hr)), m_hr(hr) { }
    HRESULT error()  const { return m_hr; }
private:
    const HRESULT m_hr;
};

/**
* @brief 失敗したら例外を投げる
*/
inline void throwIfFailed(HRESULT hr) {
    if (FAILED(hr)) {
        throw HrException(hr);
    }
}

/**
* @brief ポインタがnullなら例外を投げる
*/
inline void throwIfNull(void* ptr) {
    if (!ptr) {
        throw std::exception("nullptr!");
    }
}

} //Graphics 
} //Framework 