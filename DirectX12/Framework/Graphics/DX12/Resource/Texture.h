#pragma once
#include <string>
#include <d3d12.h>
#include "Framework/Utility/Typedef.h"

namespace Framework {
namespace Graphics {

/**
* @class Texture
* @brief �e�N�X�`���N���X
*/
class Texture {
public:
    /**
    * @brief �R���X�g���N�^
    */
    Texture(const std::string& filepath);
    /**
    * @brief �f�X�g���N�^
    */
    ~Texture();
    /**
    * @brief �R�}���h���X�g�ɒǉ�����
    * @param commandList �R�}���h���X�g
    * @param rootParameterIndex ���[�g�p�����[�^�̃C���f�b�N�X
    */
    void addToCommandList(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex);
    /**
    * @brief �f�B�X�N���v�^�q�[�v�̎擾
    */
    ID3D12DescriptorHeap* getDescriptorHeap() const { return mTextureDH.Get(); }
private:
    static constexpr UINT TEXTURE_PIXEL_SIZE = 4; //!< �e�N�X�`���̃s�N�Z���T�C�Y
    ComPtr<ID3D12DescriptorHeap> mSRVHeap; //!< �e�N�X�`��SRV�p
    ComPtr<ID3D12Resource> mTexture; //!< �e�N�X�`��
    ComPtr<ID3D12DescriptorHeap> mTextureDH; //!< �e�N�X�`���̃f�B�X�N���v�^�q�[�v
};

} //Graphics 
} //Framework 