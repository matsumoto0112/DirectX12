#include "RootSignature.h"
#include <algorithm>
#include "Framework/Graphics/DX12/DXInterfaceAccessor.h"
#include "Framework/Graphics/DX12/Helper.h"
#include "Framework/Utility/Debug.h"

namespace {

D3D12_DESCRIPTOR_RANGE1 createRange(D3D12_DESCRIPTOR_RANGE_TYPE type, UINT num, UINT baseRegisterNumber, UINT registerSpace, D3D12_DESCRIPTOR_RANGE_FLAGS flag, UINT offset = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND) {
    D3D12_DESCRIPTOR_RANGE1 range{};
    range.RangeType = type;
    range.NumDescriptors = num;
    range.BaseShaderRegister = baseRegisterNumber;
    range.RegisterSpace = registerSpace;
    range.Flags = flag;
    range.OffsetInDescriptorsFromTableStart = offset;
    return range;
};


D3D12_ROOT_PARAMETER1 createCBVParameter(UINT num, D3D12_SHADER_VISIBILITY visibility) {
    D3D12_ROOT_PARAMETER1 rootParam{};
    rootParam.ShaderVisibility = visibility;
    rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParam.Descriptor.ShaderRegister = num;
    rootParam.Descriptor.RegisterSpace = 0;
    return rootParam;
}

D3D12_ROOT_PARAMETER1 createDescriptorTableParameter(UINT num, const D3D12_DESCRIPTOR_RANGE1* ranges, D3D12_SHADER_VISIBILITY visibility) {
    D3D12_ROOT_PARAMETER1 rootParam{};
    rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParam.ShaderVisibility = visibility;
    rootParam.DescriptorTable.NumDescriptorRanges = num;
    rootParam.DescriptorTable.pDescriptorRanges = ranges;
    return rootParam;
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

RootSignature::~RootSignature() { }

void RootSignature::createDX12RootSignature() {
    //const int size = mTextureParameterInfos.size();
    //std::vector<D3D12_DESCRIPTOR_RANGE1> ranges(size);
    //for (int i = 0; i < size; i++) {
    //    ranges[i] = createRange(D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
    //        1,
    //        mTextureParameterInfos[i].registerNum,
    //        0,
    //        D3D12_DESCRIPTOR_RANGE_FLAGS::D3D12_DESCRIPTOR_RANGE_FLAG_NONE);
    //}

    //int index = 0;
    //for (int i = 0; i < mRootParameters.size(); i++) {
    //    if (mRootParameters[i].ParameterType == D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE) {
    //        mRootParameters[i].DescriptorTable.NumDescriptorRanges = 1;
    //        mRootParameters[i].DescriptorTable.pDescriptorRanges = &ranges[index++];
    //    }
    //}
    //D3D12_ROOT_SIGNATURE_FLAGS  rootSignatureFlags =
    //    D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    //D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc = createRootSignatureDesc(
    //    mRootParameters.size(),
    //    mRootParameters.data(),
    //    mSamplers.size(),
    //    mSamplers.data(),
    //    rootSignatureFlags);

    //ComPtr<ID3DBlob> sigunature, error;
    //throwIfFailed(D3D12SerializeVersionedRootSignature(&rootSignatureDesc, &sigunature, &error));

    //throwIfFailed(DXInterfaceAccessor::getDevice()->CreateRootSignature(0,
    //    sigunature->GetBufferPointer(),
    //    sigunature->GetBufferSize(),
    //    IID_PPV_ARGS(&mRootSignature)));
}

void RootSignature::addToCommandList(ID3D12GraphicsCommandList* commandList) {
    commandList->SetGraphicsRootSignature(getRootSignature());
}

void RootSignature::addConstantBufferParameter(VisibilityType visibility, UINT registerNum) {
    D3D12_ROOT_PARAMETER1 param = createCBVParameter(registerNum, toD3D12_SHADER_VISIBILITY(visibility));
    mRootParameters.emplace_back(param);
}

void RootSignature::addTextureParameter(VisibilityType visibility, UINT registerNum) {
    D3D12_ROOT_PARAMETER1 param = createDescriptorTableParameter(0, nullptr, toD3D12_SHADER_VISIBILITY(visibility));
    mRootParameters.emplace_back(param);

    mTextureParameterInfos.emplace_back(TextureParameterInfo{ (UINT)(mRootParameters.size() - 1),registerNum });
}

void RootSignature::addSamplerParameter(VisibilityType visibility, UINT registerNum) { }

void RootSignature::addStaticSamplerParameter(const D3D12_STATIC_SAMPLER_DESC& sampler) {
    mSamplers.emplace_back(sampler);
}

} //Graphics 
} //Framework 
