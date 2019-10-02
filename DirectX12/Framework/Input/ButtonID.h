#pragma once

#include <Windows.h>
namespace Framework {
namespace Input {

/**
* @enum ButtonID
* @brief ボタン番号
*/
enum class ButtonID : UINT {
    Button_NONE = 0,
    Button0 = 1,
    Button1 = Button0 << 1,
    Button2 = Button1 << 1,
    Button3 = Button2 << 1,
    Button4 = Button3 << 1,
    Button5 = Button4 << 1,
    Button6 = Button5 << 1,
    Button7 = Button6 << 1,
    Button8 = Button7 << 1,
    Button9 = Button8 << 1,
    Button10 = Button9 << 1,
    Button11 = Button10 << 1,
    Button12 = Button11 << 1,
    Button13 = Button12 << 1,
    Button14 = Button13 << 1,
    Button15 = Button14 << 1,

    Left,
    Right,
    Down,
    Up,
};

/**
* @brief コントローラ軸ID
*/
enum class AxisID {
    LeftHorizontal,
    LeftVertical,
    RightHorizontal,
    RightVertical,
    Trigger,
};

/**
* @brief コントローラ方向ID
*/
enum class DirectionID {
    Left,
    Right,
    Down,
    Up,
};

} //Input 
} //Framework
