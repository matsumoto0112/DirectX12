#pragma once
#include <d3d12.h>
#include "Framework/Utility/Typedef.h"
#include "Framework/Graphics/DX12/Shader/PixelShader.h"
#include "Framework/Graphics/DX12/Shader/VertexShader.h"

namespace Framework {
namespace Graphics {

/**
* @class Pipeline
* @brief グラフィックスパイプライン管理クラス
*/
class Pipeline {
public:
    /**
    * @brief コンストラクタ
    */
    Pipeline();
    /**
    * @brief デストラクタ
    */
    ~Pipeline();
    /**
    * @brief パイプラインを作成する
    */
    void createPipeline();
    /**
    * @brief 頂点シェーダーをセットする
    */
    void setVertexShader(VertexShader* vs);
    /**
    * @brief ピクセルシェーダーをセットする
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