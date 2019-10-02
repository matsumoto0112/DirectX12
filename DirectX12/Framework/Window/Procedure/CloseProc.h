#pragma once

#include "Framework/Window/Procedure/IWindowProc.h"

namespace Framework {
namespace Window {

/**
* @class CloseProc
* @brief 終了するときに呼ばれるプロシージャ
*/
class CloseProc :public IWindowProc{
public:
    /**
    * @brief デストラクタ
    */
    virtual ~CloseProc();
    /**
    * @brief ウィンドウプロシージャ
    */
    virtual LRESULT CALLBACK wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, BOOL* isReturn) override;
};
} //Window 
} //Framework 
