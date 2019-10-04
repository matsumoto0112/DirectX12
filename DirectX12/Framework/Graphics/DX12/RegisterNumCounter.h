#pragma once
#include <d3d12.h>
#include "Framework/Utility/Property.h"

namespace Framework {
namespace Graphics {

/**
* @class RegisterNumCounter
* @brief ���W�X�^�[�ԍ��J�E���^�[
*/
class RegisterNumCounter {
public:
    /**
    * @brief �R���X�g���N�^
    */
    RegisterNumCounter()
        :mNumber(0) { }
    /**
    * @brief �f�X�g���N�^
    */
    ~RegisterNumCounter() { };
    /**
    * @brief ���p�\�Ȕԍ����擾����
    */
    UINT getNumber() {
        UINT res = mNumber;
        mNumber++;
        return res;
    }
private:
    UINT mNumber;
};

} //Graphics 
} //Framework 