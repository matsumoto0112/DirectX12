#pragma once
#include <d3d12.h>
#include "Framework/Utility/Typedef.h"
#include "Framework/Utility/Debug.h"

namespace Framework {
namespace Graphics {

/**
* @class ConstantBuffer
* @brief �R���X�^���g�o�b�t�@�Ǘ�
*/
class ConstantBuffer {
public:
    /**
    * @brief �R���X�g���N�^
    */
    ConstantBuffer(UINT reservationNum);
    /**
    * @brief �f�X�g���N�^
    */
    ~ConstantBuffer();

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
    ComPtr<ID3D12Resource> mConstantBuffer; //!< �o�b�t�@���\�[�X
    ComPtr<ID3D12DescriptorHeap> mCBVHeap; //!< �q�[�v
    UINT mAssignedNum; //!< ���݊��蓖�ĂĂ��鐔
    UINT mOffset; //!< �o�b�t�@�̃������I�t�Z�b�g
    UINT mReservationNum; //!< �R���X�^���g�o�b�t�@�̊��蓖�ė\��
    struct { char buf[256]; }*mCBVDataBegin; //!< �������̊J�n�n�_
    bool mUpdateFlag;
};

} //Graphics 
} //Framework 