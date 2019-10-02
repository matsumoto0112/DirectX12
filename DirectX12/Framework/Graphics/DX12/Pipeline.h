#pragma once
#include <d3d12.h>
#include "Framework/Utility/Typedef.h"
#include "Framework/Graphics/DX12/Shader/PixelShader.h"
#include "Framework/Graphics/DX12/Shader/VertexShader.h"

namespace Framework {
namespace Graphics {

/**
* @class Pipeline
* @brief �O���t�B�b�N�X�p�C�v���C���Ǘ��N���X
*/
class Pipeline {
public:
    /**
    * @brief �R���X�g���N�^
    */
    Pipeline();
    /**
    * @brief �f�X�g���N�^
    */
    ~Pipeline();
    /**
    * @brief �p�C�v���C�����쐬����
    */
    void createPipeline();
    /**
    * @brief ���_�V�F�[�_�[���Z�b�g����
    */
    void setVertexShader(VertexShader* vs);
    /**
    * @brief �s�N�Z���V�F�[�_�[���Z�b�g����
    */
    void setPixelShader(PixelShader* ps);

    ID3D12PipelineState* getPipelicneState() const;
    ID3D12RootSignature* getRoot() const;
private:
    D3D12_GRAPHICS_PIPELINE_STATE_DESC mPipelineDesc;
    ComPtr<ID3D12PipelineState> mPipeline;
    ComPtr<ID3D12RootSignature> mRoot;
};

} //Graphics 
} //Framework 