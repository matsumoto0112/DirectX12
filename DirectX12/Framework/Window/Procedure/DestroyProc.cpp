#include "DestroyProc.h"

namespace Framework {
namespace Window {

DestroyProc::~DestroyProc() {}

LRESULT DestroyProc::wndProc(HWND hWnd, UINT msg,
    WPARAM wParam, LPARAM lParam, BOOL* isReturn) {
    switch (msg) {
    case WM_DESTROY:
        *isReturn = TRUE;
        PostQuitMessage(0);
        break;
    default:
        break;
    }
    return 0L;
}

} //Window 
} //Framework 
