#pragma once
namespace Framework {
namespace Input {

/**
* @brief �R���g���[���Ɋւ��郆�[�e�B���e�B�N���X
*/
class ControllerUtil {
public:
    static constexpr unsigned int AXIS_MAX_POSITION = 65535;; //!< ���̍ő���W
    static constexpr float HALF_AXIS_MAX_POSITION = AXIS_MAX_POSITION * 0.5f;; //!< ���̍ő���W�̔���
};
} //Input 
} //Framework
