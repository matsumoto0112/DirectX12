#include "RootSignature.h"
#include "Framework/Define/Render.h"
#include "Framework/Graphics/DX12/DXInterfaceAccessor.h"
#include "Framework/Graphics/DX12/Helper.h"
#include "Framework/Utility/Debug.h"

namespace {
D3D12_DESCRIPTOR_RANGE1 createRange(D3D12_DESCRIPTOR_RANGE_TYPE type,
    UINT num,
    UINT baseRegisterNumber,
    UINT registerSpace,
    D3D12_DESCRIPTOR_RANGE_FLAGS flag,
    UINT offset = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND) {
    D3D12_DESCRIPTOR_RANGE1 range{};
    range.RangeType = type;
    range.NumDescriptors = num;
    range.BaseShaderRegister = baseRegisterNumber;
    range.RegisterSpace = registerSpace;
    range.Flags = flag;
    range.OffsetInDescriptorsFromTableStart = offset;
    return range;
};

D3D12_VERSIONED_ROOT_SIGNATURE_DESC createRootSignatureDesc(UINT num,
    const D3D12_ROOT_PARAMETER1* param,
    UINT numStaticSampler,
    const D3D12_STATIC_SAMPLER_DESC* sampler,
    D3D12_ROOT_SIGNATURE_FLAGS flag) {
    D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    rootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION::D3D_ROOT_SIGNATURE_VERSION_1;
    rootSignatureDesc.Desc_1_1.NumParameters = num;
    rootSignatureDesc.Desc_1_1.pParameters = param;
    rootSignatureDesc.Desc_1_1.NumStaticSamplers = numStaticSampler;
    rootSignatureDesc.Desc_1_1.pStaticSamplers = sampler;
    rootSignatureDesc.Desc_1_1.Flags = flag;
    return rootSignatureDesc;
};
}

namespace Framework {
namespace Graphics {

RootSignature::RootSignature() { }

RootSignature::~RootSignature() {
    mSamplers.clear();
}

void RootSignature::createDefaultRootSignature() {
    D3D12_DESCRIPTOR_RANGE1 CBRange[1];
    CBRange[0] = createRange(D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
        Define::Render::MAX_CONSTANT_BUFFER_USE_NUM_PER_ONE_FRAME, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAGS::D3D12_DESCRIPTOR_RANGE_FLAG_NONE);
    D3D12_DESCRIPTOR_RANGE1 TEXRange[1];
    TEXRange[0] = createRange(D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
        Define::Render::MAX_TEXTURE_USE_NUM_PER_ONE_FRAME, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAGS::D3D12_DESCRIPTOR_RANGE_FLAG_NONE);

    D3D12_ROOT_PARAMETER1 pamameter[2]{};
    pamameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    pamameter[0].DescriptorTable.NumDescriptorRanges = 1;
    pamameter[0].DescriptorTable.pDescriptorRanges = &CBRange[0];
    pamameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL;

    pamameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    pamameter[1].DescriptorTable.NumDescriptorRanges = 1;
    pamameter[1].DescriptorTable.pDescriptorRanges = &TEXRange[0];
    pamameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL;

    D3D12_VERSIONED_ROOT_SIGNATURE_DESC desc = createRootSignatureDesc(
        _countof(pamameter),
        pamameter,
        mSamplers.size(),
        mSamplers.data(),
        D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ComPtr<ID3DBlob> sigunature, error;
    throwIfFailed(D3D12SerializeVersionedRootSignature(&desc, &sigunature, &error));

    throwIfFailed(DXInterfaceAccessor::getDevice()->CreateRootSignature(0,
        sigunature->GetBufferPointer(),
        sigunature->GetBufferSize(),
        IID_PPV_ARGS(&mRootSignature)));
}

void RootSignature::addToCommandList(ID3D12GraphicsCommandList* commandList) {
    commandList->SetGraphicsRootSignature(getRootSignature());
}

void RootSignature::addStaticSamplerParameter(const D3D12_STATIC_SAMPLER_DESC& sampler) {
    mSamplers.emplace_back(sampler);
}

} //Graphics 
} //Framework 
