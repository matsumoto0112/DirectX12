#pragma once
#include <vector>
#include "Framework/Input/KeyCode.h"
#include "Framework/Utility/Property.h"
#include "Framework/Utility/Singleton.h"

namespace Framework {
namespace Define {

/**
* @class Config
* @brief �Q�[���̐ݒ���
*/
class Config : public Utility::Singleton<Config> {
public:
protected:
    /**
    * @brief �R���X�g���N�^
    */
    Config();
    /**
    * @brief �f�X�g���N�^
    */
    ~Config();
private:
    int mScreenWidth; //!< �X�N���[����
    int mScreenHeight; //!< �X�N���[������
    bool mKeepFPS; //!< FPS��ۂ悤�ɂ��邩
    float mFPS; //!< �ڕWFPS
    std::vector<Input::KeyCode> mGameEndKeys; //!< �Q�[�����I������L�[
public:
    Utility::GetterProperty<int> screenWidth{ mScreenWidth };
    Utility::GetterProperty<int> screenHeight{ mScreenHeight };
    Utility::GetterProperty<bool> keepFPS{ mKeepFPS };
    Utility::GetterProperty<float> FPS{ mFPS };
    Utility::GetRefProperty<std::vector<Input::KeyCode>> gameEndKeys{ mGameEndKeys };
};

} //Define 
} //Framework 