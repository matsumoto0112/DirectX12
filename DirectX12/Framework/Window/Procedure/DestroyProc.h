#pragma once

#include "Framework/Window/Procedure/IWindowProc.h"

namespace Framework {
namespace Window {
/**
* @class DestroyProc
* @brief ウィンドウが破棄されるときに呼ばれるプロシージャ
*/
class DestroyProc :public IWindowProc{
public:
    /**
    * @brief デストラクタ
    */
    virtual ~DestroyProc();
    /**
    * @brief ウィンドウプロシージャ
    */
    virtual LRESULT CALLBACK wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, BOOL* isReturn) override;
};

} //Window 
} //Framework 
