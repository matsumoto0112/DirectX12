#include "Window.h"
#include "Framework/Utility/Debug.h"
#include "Framework/Window/WindowProcedures.h"

namespace {
/**
* @brief ウィンドウを作成する
* @param hInstance インスタンスハンドル
* @param hWnd ウィンドウハンドル
* @param clientSize クライアント領域の大きさ
* @param position ウィンドウの場所
* @param name ウィンドウタイトル
*/
bool createWindow(HINSTANCE hInstance,
    HWND* hWnd,
    const Framework::Math::Vector2& clientSize,
    const Framework::Math::Vector2& position,
    const std::string& name) {
    // ウィンドウクラスのパラメータ設定
    WNDCLASSEX wcex;
    //構造体のサイズ
    wcex.cbSize = sizeof(WNDCLASSEX);
    //ウィンドウスタイル
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    //ウィンドウのメッセージ処理をするコールバック関数の登録
    wcex.lpfnWndProc = Framework::Window::WindowProcedures::mainWndProc;
    //ウインドウクラス構造体の後ろに割り当てる補足バイト数
    wcex.cbClsExtra = 0;
    //ウインドウインスタンスの後ろに割り当てる補足バイト数
    wcex.cbWndExtra = 0;
    //このクラスのためのウインドウプロシージャがあるインスタンスハンドル
    wcex.hInstance = hInstance;
    //アイコンのハンドル
    wcex.hIcon = NULL;
    //マウスカーソルのハンドル
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    //ウインドウ背景色
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW - 1);
    //デフォルトメニュー名
    wcex.lpszMenuName = NULL;
    //ウインドウクラスにつける名前
    wcex.lpszClassName = name.c_str();
    //16×16の小さいサイズのアイコン
    wcex.hIconSm = NULL;
    //登録に失敗したら失敗したと返す
    if (!RegisterClassEx(&wcex))
        return false;

    //ウィンドウの生成 
    //とりあえず適当な大きさでウィンドウを作り、後で大きさを修正する
    *hWnd = CreateWindow(name.c_str(),
        name.c_str(),
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        0,
        0,
        800,
        600,
        nullptr,
        nullptr,
        wcex.hInstance,
        NULL);
    //登録に失敗したら失敗したと返す
    if (!(*hWnd))
        return false;

    return true;
}
}
namespace Framework {
namespace Window {

Window::Window(HINSTANCE hInstance,
    const Vec2& clientSize,
    const Vec2& position,
    const std::string& windowTitle)
    :mClientSize(clientSize) {

    bool succeed;
    succeed = createWindow(hInstance, &mHWnd, clientSize, position, windowTitle.c_str());
    //エラーチェック
    if (!succeed) {
        MY_ERROR_WINDOW(succeed, "ウィンドウの生成に失敗しました。");
        return;
    }

    setWindowSizeAndPosition(clientSize, position);
    ShowWindow(mHWnd, SW_SHOW);
    UpdateWindow(mHWnd);
}

Window::~Window() { }

void Window::setWindowSizeAndPosition(const Vec2& clientSize, const Vec2& position) {
    //最後に大きさを設定する
    SetWindowPos(mHWnd,
        nullptr,
        static_cast<int>(position.x),
        static_cast<int>(position.y),
        static_cast<int>(clientSize.x),
        static_cast<int>(clientSize.y),
        SWP_NOZORDER);
}

void Window::quit() {
    PostMessage(mHWnd, WM_CLOSE, 0, 0);
}

HWND Window::getHWND() const {
    return mHWnd;
}

bool Window::isClosed() const {
    return IsWindow(mHWnd) == FALSE;
}

void Window::addProcedureEvent(IWindowProc* proc) {
    //ユニークポインタ化する
    WindowProcedures::mWindowProcs.emplace_back(std::unique_ptr<IWindowProc>(proc));
}

bool Window::isActive() const {
    return GetActiveWindow() == mHWnd;
}

} //Window 
} //Framework 