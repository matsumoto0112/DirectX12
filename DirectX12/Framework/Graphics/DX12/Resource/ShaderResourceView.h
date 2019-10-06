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

    bool canUpdate() const;
    /**
    * @brief �o�b�t�@�̍X�V
    */
    void updateBuffer(ComPtr<ID3D12Resource> texture);
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
    ComPtr<ID3D12DescriptorHeap> mSRVHeap;
    const UINT mReservationNum;
    UINT mUsedOffset;
    UINT mCurrentUsedNum;
};

} //Graphics 
} //Framework 