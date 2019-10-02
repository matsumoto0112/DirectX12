#pragma once
#include <Windows.h>
namespace Framework {
namespace Window {

/**
* @class IWindowProc
* @brief ウィンドウプロシージャインターフェース
*/
class IWindowProc {
public:
    /**
    * @brief デストラクタ
    */
    virtual  ~IWindowProc() = default;
    /**
    * @brief ウィンドウプロシージャ
    */
    virtual LRESULT CALLBACK wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, BOOL* isReturn) = 0;
};
} //Window 
} //Framework 
