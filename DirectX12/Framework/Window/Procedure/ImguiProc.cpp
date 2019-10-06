#include "ImguiProc.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Framework {
namespace Window {

ImguiProc::~ImguiProc() { }

LRESULT ImguiProc::wndProc(HWND hWnd, UINT msg,
    WPARAM wParam, LPARAM lParam, BOOL* isReturn) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        *isReturn = TRUE;

    return 0L;
}


} //Window 
} //Framework 
