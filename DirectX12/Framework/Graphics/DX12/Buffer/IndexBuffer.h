#pragma once
#include <vector>
#include <d3d12.h>
#include "Framework/Utility/Typedef.h"

namespace Framework {
namespace Graphics {

/**
* @class IndexBuffer
* @brief �C���f�b�N�X�o�b�t�@
*/
class IndexBuffer {
public:
    /**
    * @brief �R���X�g���N�^
    */
    IndexBuffer(const std::vector<UINT>& indices);
    /**
    * @brief �f�X�g���N�^
    */
    ~IndexBuffer();
    /**
    * @brief �R�}���h���X�g�Ɏ��g��ǉ�����
    */
    void addToCommandList(ID3D12GraphicsCommandList* commandList);
    /**
    * @brief �`��w�߂𑗂�
    */
    void drawCall(ID3D12GraphicsCommandList* commandList);
private:
    const UINT mIndexNum; //!< �C���f�b�N�X�̐�
    ComPtr<ID3D12Resource> mIndexBuffer; //!< �C���f�b�N�X�o�b�t�@
    D3D12_INDEX_BUFFER_VIEW mIndexBufferView; //!< �C���f�b�N�X�o�b�t�@�r���[
};

} //Graphics 
} //Framework 