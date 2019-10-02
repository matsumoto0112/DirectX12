#include "Keyboard.h"
#include "Framework/Input/KeyCode.h"
#include "Framework/Utility/Debug.h"

namespace {
/**
* @brief �L�[�R�[�h���炻�̃L�[�R�[�h�ɑΉ������z��ԍ����擾����
*/
int getIndex(Framework::Input::KeyCode key) {
    return static_cast<int>(key);
}
}

namespace Framework {
namespace Input {

Keyboard::Keyboard(HWND hWnd)
    :mCurrentKeys(), mPrevKeys() { }

Keyboard::~Keyboard() { }

void Keyboard::update() {
    std::copy(mCurrentKeys.begin(), mCurrentKeys.end(), mPrevKeys.begin());
    bool success = GetKeyboardState(mCurrentKeys.data());
    if (!success) {
        mCurrentKeys.fill(0);
    }
}

bool Keyboard::getKey(KeyCode key) const {
    return checkKeyDown(mCurrentKeys, key);
}

bool Keyboard::getKeyDown(KeyCode key) const {
    bool prev = checkKeyDown(mPrevKeys, key);
    bool cur = checkKeyDown(mCurrentKeys, key);
    return !prev && cur;
}

bool Keyboard::getKeyUp(KeyCode key) const {
    bool prev = checkKeyDown(mPrevKeys, key);
    bool cur = checkKeyDown(mCurrentKeys, key);
    return prev && !cur;
}

bool Keyboard::checkKeyDown(const KeyInfo& keys, KeyCode key) const {
    return (keys[getIndex(key)] & 0x80) != 0;
}

} //Input 
} //Framework
