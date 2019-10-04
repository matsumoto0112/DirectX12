#pragma once
#include <string>
#include <vector>
#include <d3d12.h>

namespace Framework {
namespace Utility {

/**
* @class ShaderReader
* @brief discription
*/
class ShaderReader {
public:
    /**
    * @brief �R���X�g���N�^
    */
    ShaderReader(const std::string& filepath);
    /**
    * @brief �f�X�g���N�^
    */
    ~ShaderReader();
    /**
    * @brief �V�F�[�_�[�f�[�^���擾����
    */
    std::vector<BYTE> get() const;
    /**
    * @brief �V�F�[�_�[���t���N�V�������擾����
    */
    std::vector<D3D12_INPUT_ELEMENT_DESC> getShaderReflection();
private:
    std::vector<BYTE> mData; //!< �ǂݍ��񂾃f�[�^
    std::vector<D3D12_INPUT_ELEMENT_DESC> mLayout;
};

} //Utility 
} //Framework 