#pragma once
#include <unordered_map>
#include <Windows.h>
#include "Framework/Input/KeyCode.h"
#include "Framework/Math/Vector2.h"

namespace Framework {
namespace Window {
class Window;
} //Window 

namespace Input {
class Keyboard;

/**
* @enum MouseButton
* @brief マウスのボタン定義
*/
enum class MouseButton {
    Left = static_cast<int>(KeyCode::LButton),
    Right = static_cast<int>(KeyCode::RButton),
    Middle = static_cast<int>(KeyCode::MButton),
};

/**
* @enum CursorMode
* @brief マウスカーソルの状態定義
*/
enum class CursorMode {
    Show,
    Hide,
};

/**
* @class Mouse
* @brief マウス情報管理クラス
*/
class Mouse {
public:
    /**
    * @brief コンストラクタ
    * @param window メインウィンドウ
    * @param keyboard キーボード
    * @details 入力の取得はキーボードと一緒に行うためキーボードが必要
    */
    Mouse(Window::Window& window, Keyboard& keyboard);
    /**
    * @brief デストラクタ
    */
    ~Mouse();
    /**
    * @brief 更新
    */
    void update();
    /**
    * @brief マウスの座標を取得
    * @return クライアント領域でのマウス座標を返す
    */
    const Math::Vector2& getMousePosition() const;
    /**
    * @brief マウスのボタンを押しているか
    * @param button ボタンの種類
    * @return 押している状態ならtrueを返す
    */
    bool getMouse(MouseButton button) const;
    /**
    * @brief マウスのボタンを押したか
    * @param button ボタンの種類
    * @return 押したタイミングならtrueを返す
    */
    bool getMouseDown(MouseButton button) const;
    /**
    * @brief マウスのボタンを離したか
    * @param button ボタンの種類
    * @return 離したタイミングならtrueを返す
    */
    bool getMouseUp(MouseButton button) const;
    /**
    * @brief マウスの移動量を取得
    */
    Math::Vector2 getMove() const;
private:
    Window::Window& mMainWindow;
    Keyboard& mKeyboard; //!< キーボード
    Math::Vector2 mPosition; //!< 今のマウスの座標
    Math::Vector2 mPrevPosition; //!< 前のマウスの座標
};

} //Input 
} //Framework
