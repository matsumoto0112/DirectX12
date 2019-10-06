#include "WindowProcedures.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Framework {
namespace Window {

std::vector<std::unique_ptr<IWindowProc>>
WindowProcedures::mWindowProcs = std::vector<std::unique_ptr<IWindowProc>>();

LRESULT WindowProcedures::mainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;

     BOOL isReturn = FALSE;
    //先頭から順にプロシージャが処理されるまでループ
    for (auto&& proc : mWindowProcs) {
        LRESULT res = proc->wndProc(hWnd, message, wParam, lParam, &isReturn);
        if (isReturn)return res;
    }
    //既定の処理を行う
    return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT WindowProcedures::subWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    return DefWindowProc(hWnd, message, wParam, lParam);
}

} //Window 
} //Framework 
