#include "Mouse.h"
#include "Framework/Input/Keyboard.h"
#include "Framework/Window/Window.h"

namespace {
constexpr Framework::Input::KeyCode cast(Framework::Input::MouseButton button) {
    return (Framework::Input::KeyCode)button;
}

Framework::Math::Vector2 toVector2(const POINT& pos) {
    return Framework::Math::Vector2((float)pos.x, (float)pos.y);
}

POINT toPoint(const Framework::Math::Vector2& pos) {
    return { (LONG)pos.x,(LONG)pos.y };
}
}
namespace Framework {
namespace Input {

Mouse::Mouse(Window::Window& window, Keyboard& keyboard)
    :mMainWindow(window), mKeyboard(keyboard) { }

Mouse::~Mouse() { }

void Mouse::update() {
    mPrevPosition = mPosition;
    POINT pos;
    GetCursorPos(&pos);
    ScreenToClient(mMainWindow.getHWND(), &pos);
    mPosition = toVector2(pos);
}

const Math::Vector2& Mouse::getMousePosition() const {
    return mPosition;
}

bool Mouse::getMouse(MouseButton button) const {
    //メインウィンドウがアクティブ状態でなければ反応しない
    if (!mMainWindow.isActive())return false;
    return mKeyboard.getKey(cast(button));
}

bool Mouse::getMouseDown(MouseButton button) const {
    //メインウィンドウがアクティブ状態でなければ反応しない
    if (!mMainWindow.isActive())return false;
    return mKeyboard.getKeyDown(cast(button));
}

bool Mouse::getMouseUp(MouseButton button) const {
    //メインウィンドウがアクティブ状態でなければ反応しない
    if (!mMainWindow.isActive())return false;
    return mKeyboard.getKeyUp(cast(button));
}

Math::Vector2 Mouse::getMove() const {
    //メインウィンドウがアクティブ状態でなければ反応しない
    if (!mMainWindow.isActive())return Math::Vector2::ZERO;

    return mPosition - mPrevPosition;
}

} //Input 
} //Framework
