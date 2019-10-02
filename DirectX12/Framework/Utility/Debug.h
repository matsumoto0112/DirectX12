#pragma once

#include <string>
#include "Framework/Utility/StringBuilder.h"

/**
* @def VisualStudioの出力にログ出力
* @param mes 表示するメッセージ
*/
#define MY_DEBUG_LOG(mes) Framework::Utility::Debug::debugLog(__FUNCTION__,__LINE__, mes)

/**
* @def アサーション
* @param cond 条件式
* @param mes condがfalseの時に表示するメッセージ
*/
#define MY_ASSERTION(cond,mes) Framework::Utility::Debug::assertion(__FUNCTION__,__LINE__,cond, mes);

/**
* @def エラーウィンドウの表示
* @param cond 条件式
* @param mes condがfalseの時に表示するメッセージ
* @details デバッグ時にはメッセージとともに発生した関数名、行番号が表示される
*/
#define MY_ERROR_WINDOW(cond,mes) Framework::Utility::Debug::errorWindow(__FUNCTION__,__LINE__,cond,mes);

/**
* @def 条件を満たしていたらVisualStudioの出力にログ出力
* @param cond 条件式
* @param mes 表示するメッセージ
*/
#define MY_DEBUG_LOG_IF(cond,mes) if((cond)) MY_DEBUG_LOG(mes)

namespace Framework {
namespace Utility {
class Debug {
public:
    /**
    * @brief アサーション
    * @param funcName 関数名
    * @param line 発生した行
    * @param condition 条件
    * @param message エラーメッセージ
    */
    static void assertion(const std::string& funcName, int line,
        bool condition, const std::string& message);
    /**
    * @brief ログ出力
    * @param funcName 関数名
    * @param line 発生した行
    * @param message 出力内容
    */
    static void debugLog(const std::string& funcName, int line, const std::string& message);
    /**
    * @brief エラーウィンドウの表示
    * @param funcName 関数名
    * @param line 発生した行
    * @param condition 条件式
    * @param message エラーメッセージ
    */
    static void errorWindow(const std::string& funcName, int line,
        bool condition, const std::string& message);
    /**
    * @brief エラーメッセージの作成
    * @param funcName 関数名
    * @param line 発生した行
    * @param message 実際のエラーメッセージ
    * @return [funcName line:message]の形式のメッセージを返す
    */
    static std::string errorMessage(const std::string& funcName, int line,
        const std::string& message);
};

} //Utility
} //Framework