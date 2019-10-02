#pragma once

#include <Windows.h>
#include <vector>
#include <memory>
#include <string>
#include "Framework/Utility/Singleton.h"

namespace Framework {
namespace Window {
class Window;
} //Window 
namespace Input {
class InputManager;
} //Input 

namespace Device {
/**
* @class GameDevice
* @brief ゲーム機器管理
*/
class GameDevice : public Utility::Singleton<GameDevice> {
private:
    using WindowPtr = std::unique_ptr<Window::Window>;
    using InputManagerPtr = std::unique_ptr<Input::InputManager>;
public:
    /**
    * @brief デバイスの更新
    */
    void update();
    /**
    * @brief ゲームが終了しているか
    */
    bool isEnd() const;
    /**
    * @brief 終了処理
    */
    void finalize();
    /**
    * @brief 初期化処理
    */
    void initialize(const std::string& title, HINSTANCE hInstance);
protected:
    /**
    * @brief コンストラクタ
    */
    GameDevice();
    /**
    * @brief デストラクタ
    */
    virtual ~GameDevice();
public:
    /**
    * @brief メインウィンドウの取得
    */
    Window::Window* getWindow() const;
    /**
    * @brief 入力管理デバイスの取得
    */
    Input::InputManager* getInputManager() const;
private:
    WindowPtr mMainWindow; //!< メインウィンドウ
    InputManagerPtr mInputManager; //!< 入力管理
};

} //Device 
} //Framework 