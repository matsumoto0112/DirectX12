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
* @brief リソースの種類
*/
enum class ResourceType {
    ConstantBuffer,
    Texture,
    Sampler,
};

/**
* @brief テクスチャパラメータを作成するのに必要なデータ軍
*/
struct TextureParameterInfo {
    UINT paramIndex;
    UINT registerNum;
};


/**
* @class RootSignature
* @brief ルートシグネチャクラス
*/
class RootSignature {
public:
    /**
    * @brief コンストラクタ
    */
    RootSignature();
    /**
    * @brief デストラクタ
    */
    ~RootSignature();
    /**
    * @brief ルートシグネチャオブジェクトを作成する
    */
    void createDX12RootSignature();
    /**
    * @brief ルートシグネチャに取得する
    */
    ID3D12RootSignature* getRootSignature() const { return mRootSignature.Get(); }
    /**
    * @brief コマンドリストに追加する
    */
    void addToCommandList(ID3D12GraphicsCommandList* commandList);
    /**
    * @brief コンスタントパラメータを追加する
    */
    void addConstantBufferParameter(VisibilityType visibility, UINT shaderRegisterNum);
    /**
    * @brief テクスチャパラメータを追加する
    */
    void addTextureParameter(VisibilityType visibility, UINT shaderRegisterNum);
    /**
    * @brief サンプラーパラメータを追加する
    */
    void addSamplerParameter(VisibilityType visibility, UINT shaderRegisterNum);
    /**
    * @brief スタティックサンプラーを追加する
    */
    void addStaticSamplerParameter(const D3D12_STATIC_SAMPLER_DESC& sampler);
//private:
    D3D12_VERSIONED_ROOT_SIGNATURE_DESC mRootSignatureDesc;
    ComPtr<ID3D12RootSignature> mRootSignature;
    std::vector<D3D12_ROOT_PARAMETER1> mRootParameters; //!< ルートパラメータ配列
    std::vector<TextureParameterInfo> mTextureParameterInfos; //!< テクスチャパラメータを作成するためのデータ
    std::vector<D3D12_STATIC_SAMPLER_DESC> mSamplers; //!< スタティックサンプラー配列
};

} //Graphics 
} //Framework 