#pragma once
#include <d3d12.h>
#include "Framework/Utility/Typedef.h"
#include "Framework/Utility/Debug.h"

namespace Framework {
namespace Graphics {

/**
* @class ShaderResourceView
* @brief �V�F�[�_�[���\�[�X�r���[
*/
class ShaderResourceView {
public:
    /**
    * @brief �R���X�g���N�^
    */
    ShaderResourceView(UINT reservationNum);
    /**
    * @brief �f�X�g���N�^
    */
    ~ShaderResourceView();

    bool canUpdate(UINT size) const;
    /**
    * @brief �o�b�t�@�̍X�V
    */
    void updateBuffer(void* buf, UINT size);
    /**
    * @brief �o�b�t�@�f�[�^�̍X�V�J�n
    */
    void beginCBUpdate();
    /**
    * @brief �o�b�t�@�f�[�^�̍X�V�I��
    */
    void endCBUpdate(ID3D12GraphicsCommandList* commandList);
    /**
    * @brief �`��t���[���J�n���ɌĂ�
    */
    void beginFrame();

private:

};

} //Graphics 
} //Framework 