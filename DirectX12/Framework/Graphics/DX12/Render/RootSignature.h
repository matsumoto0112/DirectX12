#pragma once
#include <vector>
#include <d3d12.h>
#include "Framework/Utility/Typedef.h"
#include "Framework/Graphics/DX12/RegisterNumCounter.h"
#include "Framework/Graphics/DX12/Resource/ConstantBuffer.h"
#include "Framework/Graphics/DX12/Resource/Texture.h"
#include "Framework/Graphics/DX12/VisibilityType.h"

namespace Framework {
namespace Graphics {

/**
* @enum ResourceType
* @brief ���\�[�X�̎��
*/
enum class ResourceType {
    ConstantBuffer,
    Texture,
    Sampler,
};

/**
* @brief �e�N�X�`���p�����[�^���쐬����̂ɕK�v�ȃf�[�^�R
*/
struct TextureParameterInfo {
    UINT paramIndex;
    UINT registerNum;
};


/**
* @class RootSignature
* @brief ���[�g�V�O�l�`���N���X
*/
class RootSignature {
public:
    /**
    * @brief �R���X�g���N�^
    */
    RootSignature();
    /**
    * @brief �f�X�g���N�^
    */
    ~RootSignature();
    /**
    * @brief ���[�g�V�O�l�`���I�u�W�F�N�g���쐬����
    */
    void createDX12RootSignature();
    /**
    * @brief ���[�g�V�O�l�`���Ɏ擾����
    */
    ID3D12RootSignature* getRootSignature() const { return mRootSignature.Get(); }
    /**
    * @brief �R�}���h���X�g�ɒǉ�����
    */
    void addToCommandList(ID3D12GraphicsCommandList* commandList);
    /**
    * @brief �R���X�^���g�p�����[�^��ǉ�����
    */
    void addConstantBufferParameter(VisibilityType visibility, UINT shaderRegisterNum);
    /**
    * @brief �e�N�X�`���p�����[�^��ǉ�����
    */
    void addTextureParameter(VisibilityType visibility, UINT shaderRegisterNum);
    /**
    * @brief �T���v���[�p�����[�^��ǉ�����
    */
    void addSamplerParameter(VisibilityType visibility, UINT shaderRegisterNum);
    /**
    * @brief �X�^�e�B�b�N�T���v���[��ǉ�����
    */
    void addStaticSamplerParameter(const D3D12_STATIC_SAMPLER_DESC& sampler);
//private:
    D3D12_VERSIONED_ROOT_SIGNATURE_DESC mRootSignatureDesc;
    ComPtr<ID3D12RootSignature> mRootSignature;
    std::vector<D3D12_ROOT_PARAMETER1> mRootParameters; //!< ���[�g�p�����[�^�z��
    std::vector<TextureParameterInfo> mTextureParameterInfos; //!< �e�N�X�`���p�����[�^���쐬���邽�߂̃f�[�^
    std::vector<D3D12_STATIC_SAMPLER_DESC> mSamplers; //!< �X�^�e�B�b�N�T���v���[�z��
};

} //Graphics 
} //Framework 