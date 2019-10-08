#include "Pipeline.h"
#include "Framework/Graphics/DX12/DXInterfaceAccessor.h"
#include "Framework/Graphics/DX12/Helper.h"

namespace Framework {
namespace Graphics {

Pipeline::Pipeline(std::shared_ptr<RootSignature> rootSignature)
    :mPSODesc{}, mRootSignature(rootSignature) {
    mPSODesc.pRootSignature = rootSignature->getRootSignature();
}

Pipeline::Pipeline()
    : mPSODesc{} { }


Pipeline::~Pipeline() { }

void Pipeline::setVertexShader(const D3D12_SHADER_BYTECODE& vertexShader) {
    mPSODesc.VS = vertexShader;
}

void Pipeline::setPixelShader(const D3D12_SHADER_BYTECODE& pixelShader) {
    mPSODesc.PS = pixelShader;
}

void Pipeline::setInputLayout(const D3D12_INPUT_LAYOUT_DESC& inputLayout) {
    mPSODesc.InputLayout = inputLayout;
}

void Pipeline::setRasterizerState(const D3D12_RASTERIZER_DESC& rasterizer) {
    mPSODesc.RasterizerState = rasterizer;
}

void Pipeline::setBlendState(const D3D12_BLEND_DESC& blendDesc) {
    mPSODesc.BlendState = blendDesc;
}

void Pipeline::setDepthStencil(const D3D12_DEPTH_STENCIL_DESC& depthStencil, DXGI_FORMAT format) {
    mPSODesc.DepthStencilState = depthStencil;
    mPSODesc.DSVFormat = format;
}

void Pipeline::setPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType) {
    mPSODesc.PrimitiveTopologyType = topologyType;
}

void Pipeline::setSampleDesc(const DXGI_SAMPLE_DESC& sampleDesc) {
    mPSODesc.SampleDesc = sampleDesc;
}

void Pipeline::setSampleMask(UINT mask) {
    mPSODesc.SampleMask = mask;
}

void Pipeline::setRenderTarget(const std::vector<DXGI_FORMAT>& formats) {
    UINT size = formats.size();
    mPSODesc.NumRenderTargets = size;
    for (UINT i = 0; i < size; i++) {
        mPSODesc.RTVFormats[i] = formats[i];
    }
}

void Pipeline::createPipelineState() {
    throwIfFailed(DXInterfaceAccessor::getDevice()->CreateGraphicsPipelineState(&mPSODesc, IID_PPV_ARGS(&mPipelineState)));
}

void Pipeline::addToCommandList(ID3D12GraphicsCommandList* commandList) {
    commandList->SetPipelineState(mPipelineState.Get());
}

} //Graphics 
} //Framework 
