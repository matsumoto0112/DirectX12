#pragma once
#include <memory>
#include "Framework/Graphics/DX12/Render/RootSignature.h"

namespace Framework {
namespace Graphics {

/**
* @class Pipeline
* @brief �����_�����O�p�C�v���C���Ǘ�
*/
class Pipeline {
public:
    /**
    * @brief �R���X�g���N�^
    */
    Pipeline(std::shared_ptr<RootSignature> rootSignature);
    /**
    * @brief �R���X�g���N�^
    */
    Pipeline();
    /**
    * @brief �f�X�g���N�^
    */
    ~Pipeline();

    void setVertexShader(const D3D12_SHADER_BYTECODE& vertexShader);
    void setPixelShader(const D3D12_SHADER_BYTECODE& pixelShader);
    void setInputLayout(const D3D12_INPUT_LAYOUT_DESC& inputLayout);
    void setRasterizerState(const D3D12_RASTERIZER_DESC& rasterizer);
    void setBlendState(const D3D12_BLEND_DESC& blendDesc);
    void setDepthStencil(const D3D12_DEPTH_STENCIL_DESC& depthStencil);
    void setPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType);
    void setSampleDesc(const DXGI_SAMPLE_DESC& sampleDesc);
    void setSampleMask(UINT mask);
    void setRenderTarget(const std::vector<DXGI_FORMAT>& formats);
    void createPipelineState();
    ID3D12PipelineState* getPipelineState() const { return mPipelineState.Get(); }
    void addToCommandList(ID3D12GraphicsCommandList* commandList);
//private:
    D3D12_GRAPHICS_PIPELINE_STATE_DESC mPSODesc;
    ComPtr<ID3D12PipelineState> mPipelineState; //!< �p�C�v���C���X�e�[�g
    std::shared_ptr<RootSignature> mRootSignature;
};

} //Graphics 
} //Framework 