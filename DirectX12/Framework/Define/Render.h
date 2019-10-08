#pragma once
#include <d3d12.h>

namespace Framework {
namespace Define {

/**
* @class Render
* @brief �`�����`
*/
class Render {
public:
    static constexpr UINT FRAME_COUNT = 3;
    static constexpr UINT MAX_CONSTANT_BUFFER_REGISTER_NUM = 16; //!< �R���X�^���g�o�b�t�@�̃��W�X�^�[�ő吔
    static constexpr UINT MAX_TEXTURE_REGISTER_NUM = 8; //!< �e�N�X�`���̃��W�X�^�[�ő吔
    static constexpr UINT MAX_ONE_FRAME_RENDERING_OBJECT = 10000; //!< 1�t���[���ŕ`�悷��ő�I�u�W�F�N�g���i�����R���X�^���g�o�b�t�@��e�N�X�`���𗘗p����ꍇ�����ƕ`��\�j
    static constexpr UINT MAX_CONSTANT_BUFFER_USE_NUM_PER_ONE_FRAME = MAX_CONSTANT_BUFFER_REGISTER_NUM * MAX_ONE_FRAME_RENDERING_OBJECT; //!< 1�t���[���Ŏg�p����ő�R���X�^���g�o�b�t�@��
    static constexpr UINT MAX_TEXTURE_USE_NUM_PER_ONE_FRAME = MAX_TEXTURE_REGISTER_NUM * MAX_ONE_FRAME_RENDERING_OBJECT; //!< 1�t���[���Ŏg�p����ő�R���X�^���g�o�b�t�@��
};

} //Define 
} //Framework 