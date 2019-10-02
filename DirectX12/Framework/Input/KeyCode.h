#pragma once
namespace Framework {
namespace Input {

/**
* @enum KeyCode
* @brief キー番号
*/
enum class KeyCode {
    Escape = VK_ESCAPE, //!< エスケープキー
    One = '1', //!< 1キー
    Two = '2', //!< 2キー
    Three = '3', //!< 3キー
    Four = '4', //!< 4キー
    Five = '5', //!< 5キー
    Six = '6', //!< 6キー
    Seven = '7', //!< 7キー
    Eight = '8', //!< 8キー
    Nine = '9', //!< 9キー
    Zero = '0', //!< 0キー
    Q = 'Q', //!< Qキー
    W = 'W', //!< Wキー
    E = 'E', //!< Eキー
    R = 'R', //!< Rキー
    T = 'T', //!< Tキー
    Y = 'Y', //!< Yキー
    U = 'U', //!< Uキー
    I = 'I', //!< Iキー
    O = 'O', //!< Oキー
    P = 'P', //!< Pキー
    Enter = VK_RETURN, //!< Enterキー
    LControl = VK_LCONTROL, //!< 左Ctrlキー
    A = 'A', //!< Aキー
    S = 'S', //!< Sキー
    D = 'D', //!< Dキー
    F = 'F', //!< Fキー
    G = 'G', //!< Gキー
    H = 'H', //!< Hキー
    J = 'J', //!< Jキー
    K = 'K', //!< Kキー
    L = 'L', //!< Lキー
    LShift = VK_LSHIFT, //!< 左Shiftキー
    Z = 'Z', //!< Zキー
    X = 'X', //!< Xキー
    C = 'C', //!< Cキー
    V = 'V', //!< Bキー
    B = 'B', //!< Bキー
    N = 'N', //!< Nキー
    M = 'M', //!< Mキー
    RShift = VK_RSHIFT, //!< 右Shiftキー
    Space = VK_SPACE, //!< Spaceキー
    Numpad7 = VK_NUMPAD7, //!< Numpad 7 キー
    Numpad8 = VK_NUMPAD8, //!< Numpad 8 キー
    Numpad9 = VK_NUMPAD9, //!< Numpad 9 キー
    Numpad4 = VK_NUMPAD4, //!< Numpad 4 キー
    Numpad5 = VK_NUMPAD5, //!< Numpad 5 キー
    Numpad6 = VK_NUMPAD6, //!< Numpad 6 キー
    Numpad1 = VK_NUMPAD1, //!< Numpad 1 キー
    Numpad2 = VK_NUMPAD2, //!< Numpad 2 キー
    Numpad3 = VK_NUMPAD3, //!< Numpad 3 キー
    Numpad0 = VK_NUMPAD0, //!< Numpad 0 キー
    RControl = VK_RCONTROL, //!< RCtrlキー
    Up = VK_UP, //!< Upキー
    Left = VK_LEFT, //!< Leftキー
    Right = VK_RIGHT, //!< Rightキー
    Down = VK_DOWN, //!< Downキー

    LButton = VK_LBUTTON, //!< マウス左ボタン
    RButton = VK_RBUTTON, //!< マウス右ボタン
    MButton = VK_MBUTTON, //!< マウス中ボタン
};

} //Input 
} //Framework
