#include "Joystick.h"
#include "Framework/Input/ControllerUtil.h"

namespace {
/**
* @brief メッセージからジョイスティック情報を取得出来ていたか判定
*/
bool canGetJoyInfo(MMRESULT mmResult) {
    return (mmResult == JOYERR_NOERROR);
}

/**
* @brief ジョイスティック情報からボタンを押していたか判定
*/
bool checkButtonDown(const JOYINFOEX& info, Framework::Input::ButtonID button) {
    using  Framework::Input::ButtonID;
    //UINTに変換し入力を調べる
    UINT btn = static_cast<UINT>(button);

    //Button::Up以降は1<<15より大きい数値になっている
    if (btn > (1 << 15)) {
        int pov = info.dwPOV;
        //povが65535の時は入力がない
        if (pov == 65535) return false;
        switch (button) {
            case ButtonID::Up:
                return pov < 9000 || pov > 27000;
            case ButtonID::Right:
                return 0 < pov && pov < 18000;
            case ButtonID::Down:
                return 9000 < pov && pov < 27000;
            case ButtonID::Left:
                return 18000 < pov && pov < 36000;
        }
    }

    //押しているボタンとの論理積で判定
    return (info.dwButtons & btn) != 0;
}

}
namespace Framework {
namespace Input {

Joystick::Joystick(ID id)
    :mID(static_cast<UINT>(id)),
    mCanGetJoyInfo(false),
    mJoyInfo(),
    mPrevJoyInfo() {
    mJoyInfo.dwSize = sizeof(JOYINFOEX);
    mJoyInfo.dwFlags = JOY_RETURNALL;
    joyGetPosEx(mID, &mJoyInfo);
}

Joystick::~Joystick() { }

void Joystick::update() {
    mPrevJoyInfo = mJoyInfo;
    MMRESULT result = joyGetPosEx(mID, &mJoyInfo);
    mCanGetJoyInfo = canGetJoyInfo(result);
}

bool Joystick::isEnabled() const {
    return mCanGetJoyInfo;
}

bool Joystick::getButton(ButtonID button) const {
    return mCanGetJoyInfo && checkButtonDown(mJoyInfo, button);
}

bool Joystick::getButtonDown(ButtonID button) const {
    //前updateでボタンを押していたか
    bool prev = checkButtonDown(mPrevJoyInfo, button);
    //今updateでボタンを押していたか
    bool cur = checkButtonDown(mJoyInfo, button);

    return mCanGetJoyInfo && !prev && cur;
}

bool Joystick::getButtonUp(ButtonID button) const {
    //前updateでボタンを押していたか
    bool prev = checkButtonDown(mPrevJoyInfo, button);
    //今updateでボタンを押していたか
    bool cur = checkButtonDown(mJoyInfo, button);

    return mCanGetJoyInfo && prev && !cur;
}

float Joystick::getAxis(AxisID axis) const {
    DWORD axisInput = 0;
    switch (axis) {
        case Input::AxisID::LeftHorizontal:
            axisInput = mJoyInfo.dwXpos;
            break;
        case Input::AxisID::LeftVertical:
            axisInput = mJoyInfo.dwYpos;
            break;
        case Input::AxisID::RightHorizontal:
            axisInput = mJoyInfo.dwUpos;
            break;
        case Input::AxisID::RightVertical:
            axisInput = mJoyInfo.dwRpos;
            break;
        case Input::AxisID::Trigger:
            axisInput = mJoyInfo.dwZpos;
            break;
        default:
            break;
    }
    return (axisInput - ControllerUtil::HALF_AXIS_MAX_POSITION) / ControllerUtil::HALF_AXIS_MAX_POSITION;
}

} //Input
} //Framework
