#pragma once
#include "Framework/Graphics/DX12/Shader/Shader.h"

namespace Framework {
namespace Graphics {
/**
* @class VertexShader
* @brief ���_�V�F�[�_�[
*/
class VertexShader : public Shader {
public:
    /**
    * @brief �R���X�g���N�^
    */
    VertexShader(const std::string& filename);
    /**
    * @brief �f�X�g���N�^
    */
    virtual ~VertexShader();
    /**
    * @brief ���̓��C�A�E�g���擾����
    */
    void getInputLayoutDesc(D3D12_GRAPHICS_PIPELINE_STATE_DESC* psoDesc);
private:
    std::vector<D3D12_INPUT_ELEMENT_DESC> mInputElementDescs;
};

} //Graphics 
} //Framework 