#include "Pipeline.h"
#include "Framework/Graphics/DX12/DXWrap.h"
#include "Framework/Graphics/DX12/Helper.h"

namespace Framework {
namespace Graphics {

Pipeline::Pipeline()
    : mPipeline(), mPipelineDesc{}{
    D3D12_STATIC_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(samplerDesc));
    samplerDesc.Filter = D3D12_FILTER::D3D12_FILTER_MIN_MAG_MIP_POINT;
    samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    samplerDesc.MipLODBias = 0;
    samplerDesc.MaxAnisotropy = 0;
    samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NEVER;
    samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR::D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
    samplerDesc.MinLOD = 0.0f;
    samplerDesc.MaxAnisotropy = D3D12_FLOAT32_MAX;
    samplerDesc.ShaderRegister = 0; //番号
    samplerDesc.RegisterSpace = 0;
    samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_PIXEL; //ピクセルシェーダーで使用


    D3D12_DESCRIPTOR_RANGE1 range[1];
    range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    range[0].NumDescriptors = 1;
    range[0].BaseShaderRegister = 0;
    range[0].RegisterSpace = 0;
    range[0].Flags = D3D12_DESCRIPTOR_RANGE_FLAGS::D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC;
    range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    D3D12_ROOT_PARAMETER1 rootParameter[1];
    rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL;
    rootParameter[0].DescriptorTable.NumDescriptorRanges = 1;
    rootParameter[0].DescriptorTable.pDescriptorRanges = &range[0];

    D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    rootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION::D3D_ROOT_SIGNATURE_VERSION_1_1;
    rootSignatureDesc.Desc_1_1.NumParameters = 1;
    rootSignatureDesc.Desc_1_1.pParameters = rootParameter;
    rootSignatureDesc.Desc_1_1.NumStaticSamplers = 1;
    rootSignatureDesc.Desc_1_1.pStaticSamplers = &samplerDesc;
    rootSignatureDesc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    ComPtr<ID3D10Blob> rootSigBlob, error;
    throwIfFailed(D3D12SerializeVersionedRootSignature(&rootSignatureDesc, &rootSigBlob, &error));
    throwIfFailed(getDevice()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&mRoot)));

    mPipelineDesc.pRootSignature = mRoot.Get();
}

Pipeline::~Pipeline() { }

void Pipeline::createPipeline() {
    //マルチサンプルデスク
    mPipelineDesc.SampleDesc.Count = 1;
    mPipelineDesc.SampleMask = UINT_MAX;
    //出力レンダーターゲット数
    mPipelineDesc.NumRenderTargets = 1;
    //レンダーターゲットのフォーマット
    mPipelineDesc.RTVFormats[0] = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
    //プリミティブの形状
    mPipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    //ラスタライザの設定
    mPipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_NONE;
    mPipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE::D3D12_FILL_MODE_SOLID;
    mPipelineDesc.RasterizerState.DepthClipEnable = TRUE;
    mPipelineDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE::D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
    mPipelineDesc.RasterizerState.DepthBias = 0;
    mPipelineDesc.RasterizerState.DepthBiasClamp = 0;
    mPipelineDesc.RasterizerState.FrontCounterClockwise = FALSE;
    mPipelineDesc.RasterizerState.MultisampleEnable = FALSE;
    mPipelineDesc.RasterizerState.AntialiasedLineEnable = FALSE;


    //アルファブレンド設定
    for (int i = 0; i < _countof(mPipelineDesc.BlendState.RenderTarget); i++) {
        mPipelineDesc.BlendState.RenderTarget[i].BlendEnable = FALSE;
        mPipelineDesc.BlendState.RenderTarget[i].SrcBlend = D3D12_BLEND_ONE;
        mPipelineDesc.BlendState.RenderTarget[i].DestBlend = D3D12_BLEND_ZERO;
        mPipelineDesc.BlendState.RenderTarget[i].BlendOp = D3D12_BLEND_OP_ADD;
        mPipelineDesc.BlendState.RenderTarget[i].SrcBlendAlpha = D3D12_BLEND_ONE;
        mPipelineDesc.BlendState.RenderTarget[i].DestBlendAlpha = D3D12_BLEND_ZERO;
        mPipelineDesc.BlendState.RenderTarget[i].BlendOpAlpha = D3D12_BLEND_OP_ADD;
        mPipelineDesc.BlendState.RenderTarget[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    }
    //深度・ステンシルバッファ設定
    mPipelineDesc.DepthStencilState.DepthEnable = FALSE;

    auto device = getDevice();
    throwIfFailed(device->CreateGraphicsPipelineState(&mPipelineDesc, IID_PPV_ARGS(&mPipeline)));

}

void Pipeline::setVertexShader(VertexShader* vs) {
    mPipelineDesc.VS = vs->getShaderData();
    vs->getInputLayoutDesc(&mPipelineDesc);
}

void Pipeline::setPixelShader(PixelShader* ps) {
    mPipelineDesc.PS = ps->getShaderData();
}

ID3D12PipelineState* Pipeline::getPipelicneState() const {
    return mPipeline.Get();
}

ID3D12RootSignature* Pipeline::getRoot() const {
    return mRoot.Get();
}


} //Graphics 
} //Framework 
