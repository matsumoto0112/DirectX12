#pragma once
#include <d3d12.h>

namespace Framework {
namespace Graphics {

/**
* @class IDX12Resource
* @brief DirectX12�̃��\�[�X�C���^�[�t�F�[�X
*/
class IDX12Resource {
public:
    /**
    * @brief �f�X�g���N�^
    */
    virtual ~IDX12Resource() = default;
    /**
    * @brief �R�}���h���X�g�ɒǉ�����
    */
    virtual void addToCommandList(ID3D12GraphicsCommandList* commandList) const = 0;
};

} //Graphics 
} //Framework 