#include "WindowProcedures.h"
namespace Framework {
namespace Window {

std::vector<std::unique_ptr<IWindowProc>>
WindowProcedures::mWindowProcs = std::vector<std::unique_ptr<IWindowProc>>();

LRESULT WindowProcedures::mainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
     BOOL isReturn = FALSE;
    //先頭から順にプロシージャが処理されるまでループ
    for (auto&& proc : mWindowProcs) {
        LRESULT res = proc->wndProc(hWnd, message, wParam, lParam, &isReturn);
        if (isReturn)return res;
    }
    //既定の処理を行う
    return DefWindowProc(hWnd, message, wParam, lParam);
}

} //Window 
} //Framework 
