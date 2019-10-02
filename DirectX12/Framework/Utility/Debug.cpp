#include "Debug.h"
#include <crtdbg.h>
#include <Windows.h>
#include <sstream>

namespace Framework {
namespace Utility {

void errorWindow(bool condition, const std::string& message) {
    if (condition)return;
}

void Debug::assertion(const std::string& funcName, int line,
    bool condition, const std::string& message) {
    _ASSERT_EXPR(condition, errorMessage(funcName, line, message).c_str());
}

void Debug::debugLog(const std::string& funcName, int line, const std::string& message) {
    _RPT0(_CRT_WARN, errorMessage(funcName, line, message).c_str());
}

void Debug::errorWindow(const std::string& funcName, int line,
    bool condition, const std::string& message) {
    if (condition)return;
    std::string mes;

#if _DEBUG //デバッグ時は詳細な情報を出力する
    mes = errorMessage(funcName, line, message);
#else
    mes = message;
#endif

    MessageBox(nullptr, mes.c_str(), TEXT("エラー"), MB_ICONINFORMATION);
    exit(-1);
}

std::string Debug::errorMessage(const std::string& funcName,
    int line, const std::string& message) {
    return (StringBuilder(funcName) << " " << line << ":" << message << "\n");
}

} //Utility
} //Framework
