#include "InputManager.h"
#include "Framework/Input/Joystick.h"
#include "Framework/Input/Keyboard.h"
#include "Framework/Input/Mouse.h"
#include "Framework/Window/Window.h"
#include "Framework/Input/ID.h"
#include "Framework/Utility/Debug.h"

namespace Framework {
namespace Input {
static const int MAX_CONSTOLLER_NUM = 4;

InputManager::InputManager(Window::Window& mainWindow)
    :mKeyboard(std::make_unique<Keyboard>(mainWindow.getHWND())),
    mMouse(std::make_unique<Mouse>(mainWindow, *mKeyboard)) {
    for (UINT i = 0; i < MAX_CONSTOLLER_NUM; ++i) {
        mControllers.emplace_back(std::make_unique<Joystick>(static_cast<Input::ID>(i)));
    }
}

InputManager::~InputManager() {}

const Joystick& InputManager::getController(ID playerNumber) const {
    MY_ASSERTION(static_cast<UINT>(playerNumber) < mControllers.size(),
        "プレイヤー番号がインデックスの範囲外です。");
    return *mControllers[static_cast<UINT>(playerNumber)];
}

const Keyboard& InputManager::getKeyboard() const {
    return *mKeyboard;
}

const Mouse& InputManager::getMouse() const {
    return *mMouse;
}

Mouse& InputManager::getMouse() {
    return *mMouse;
}

void InputManager::update() {
    for (auto&& controller : mControllers) {
        controller->update();
    }
    mKeyboard->update();
    mMouse->update();
}

} //Input 
} //Framework
