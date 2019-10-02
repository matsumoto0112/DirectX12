#include "CloseProc.h"

namespace Framework {
namespace Window {

CloseProc::~CloseProc() {}

LRESULT CloseProc::wndProc(HWND hWnd, UINT msg,
    WPARAM wParam, LPARAM lParam, BOOL* isReturn) {
    switch (msg) {
    case WM_CLOSE:
        *isReturn = TRUE;
        return DefWindowProc(hWnd, msg, wParam, lParam);
    default:
        break;
    }
    return 0L;
}

} //Window 
} //Framework 
