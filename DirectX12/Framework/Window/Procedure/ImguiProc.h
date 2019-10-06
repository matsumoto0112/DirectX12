#pragma once

#include "Framework/Window/Procedure/IWindowProc.h"

namespace Framework {
namespace Window {
/**
* @class ImguiProc
* @brief ImGUI用プロシージャ
*/
class ImguiProc :public IWindowProc {
public:
    /**
    * @brief デストラクタ
    */
    virtual ~ImguiProc();
    /**
    * @brief ウィンドウプロシージャ
    */
    virtual LRESULT CALLBACK wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, BOOL* isReturn) override;
};

} //Window 
} //Framework 
