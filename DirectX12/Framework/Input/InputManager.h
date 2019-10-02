#pragma once

#include <memory>
#include <vector>
#include "Framework/Input/Keyboard.h"
#include "Framework/Input/Mouse.h"
#include "Framework/Input/Joystick.h"

namespace Framework {
namespace Window {
class Window;
} //Window 

namespace Input {
/**
* @class InputManager
* @brief 入力管理
*/
class InputManager {
private:
    using JoystickPtr = std::unique_ptr<Input::Joystick>;
    using KeyboardPtr = std::unique_ptr<Input::Keyboard>;
    using MousePtr = std::unique_ptr<Input::Mouse>;
public:
    /**
    * @brief コンストラクタ
    * @param mainWindow メインウィンドウ（キーボード、マウスの設定に必要なため）
    */
    InputManager(Window::Window& mainWindow);
    /**
    * @brief デストラクタ
    */
    ~InputManager();
    /**
    * @brief コントローラの取得
    * @param playerNumber プレイヤー番号
    */
    const Joystick& getController(ID playerNumber) const;
    /**
    * @brief キーボードの取得
    */
    const Keyboard& getKeyboard() const;
    /**
    * @brief マウスの取得
    */
    const Mouse& getMouse() const;
    /**
    * @brief マウスの取得
    */
    Mouse& getMouse();
    /**
    * @brief 更新
    */
    void update();
private:
    std::vector<JoystickPtr> mControllers; //!< コントローラ
    KeyboardPtr mKeyboard; //!< キーボード
    MousePtr mMouse; //!< マウスポインタ
};

} //Input
} //Framework
