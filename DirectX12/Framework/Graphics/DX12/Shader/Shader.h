#pragma once
#include <vector>
#include <Windows.h>
#include <string>
#include <d3d12.h>

namespace Framework {
namespace Graphics {
/**
* @class Shader
* @brief �V�F�[�_�[�N���X
*/
class Shader {
public:
    /**
    * @brief �R���X�g���N�^
    */
    Shader(const std::string& filename);
    /**
    * @brief �f�X�g���N�^
    */
    virtual ~Shader();
    /**
    * @brief �V�F�[�_�[�f�[�^���擾����
    */
    virtual D3D12_SHADER_BYTECODE getShaderData() const;
protected:
    std::vector<byte> mShaderCode; //!< �V�F�[�_�[�R�[�h
};

} //Graphics 
} //Framework 