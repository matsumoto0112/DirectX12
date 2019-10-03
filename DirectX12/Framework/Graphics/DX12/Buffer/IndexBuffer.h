#pragma once
#include <vector>
#include <d3d12.h>
#include "Framework/Graphics/DX12/IDX12Resource.h"
#include "Framework/Utility/Typedef.h"

namespace Framework {
namespace Graphics {

/**
* @class IndexBuffer
* @brief �C���f�b�N�X�o�b�t�@
*/
class IndexBuffer : public IDX12Resource {
public:
    /**
    * @brief �R���X�g���N�^
    */
    IndexBuffer(const std::vector<UINT>& indices);
    /**
    * @brief �f�X�g���N�^
    */
    virtual ~IndexBuffer();
    /**
    * @brief �R�}���h���X�g�Ɏ��g��ǉ�����
    */
    virtual void addToCommandList(ID3D12GraphicsCommandList* commandList) const override;
    /**
    * @brief �`��w�߂𑗂�
    */
    virtual void drawCall(ID3D12GraphicsCommandList* commandList);
private:
    const UINT mIndexNum; //!< �C���f�b�N�X�̐�
    ComPtr<ID3D12Resource> mIndexBuffer; //!< �C���f�b�N�X�o�b�t�@
    D3D12_INDEX_BUFFER_VIEW mIndexBufferView; //!< �C���f�b�N�X�o�b�t�@�r���[
};

} //Graphics 
} //Framework 