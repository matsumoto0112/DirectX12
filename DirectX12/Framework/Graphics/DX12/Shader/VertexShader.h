#pragma once
#include "Framework/Graphics/DX12/Shader/Shader.h"

namespace Framework {
namespace Graphics {
/**
* @class VertexShader
* @brief 頂点シェーダー
*/
class VertexShader : public Shader {
public:
    /**
    * @brief コンストラクタ
    */
    VertexShader(const std::string& filename);
    /**
    * @brief デストラクタ
    */
    virtual ~VertexShader();
    /**
    * @brief 入力レイアウトを取得する
    */
    void getInputLayoutDesc(D3D12_GRAPHICS_PIPELINE_STATE_DESC* psoDesc);
private:
    std::vector<D3D12_INPUT_ELEMENT_DESC> mInputElementDescs;
};

} //Graphics 
} //Framework 