#include "GameDevice.h"
#include <memory>
#include "Framework/Input/InputManager.h"
#include "Framework/Utility/Debug.h"
#include "Framework/Window/Window.h"
#include "Framework/Define/Config.h"

namespace Framework {
namespace Device {

Window::Window* GameDevice::getWindow() const {
    return mMainWindow.get();
}

Input::InputManager* GameDevice::getInputManager() const {
    return mInputManager.get();
}

void GameDevice::update() {
    mInputManager->update();
}

bool GameDevice::isEnd() const {
    return mMainWindow->isClosed();
}

void GameDevice::finalize() {
    mMainWindow->quit();
}

void GameDevice::initialize(const std::string& title, HINSTANCE hInstance) {
    const Math::Vector2 screenSize(
        static_cast<float>(Define::Config::getInstance().screenWidth),
        static_cast<float>(Define::Config::getInstance().screenHeight));
    mMainWindow = std::make_unique<Window::Window>(
        hInstance,
        screenSize,
        Math::Vector2(0, 0),
        title);

    mInputManager = std::make_unique<Input::InputManager>(*mMainWindow);
}

GameDevice::GameDevice() { }

GameDevice::~GameDevice() { }


} //Device 
} //Framework 