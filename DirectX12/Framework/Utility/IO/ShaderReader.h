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
    * @brief コンストラクタ
    */
    ShaderReader(const std::string& filepath);
    /**
    * @brief デストラクタ
    */
    ~ShaderReader();
    /**
    * @brief シェーダーデータを取得する
    */
    std::vector<BYTE> get() const;
    /**
    * @brief シェーダーリフレクションを取得する
    */
    std::vector<D3D12_INPUT_ELEMENT_DESC> getShaderReflection();
private:
    std::vector<BYTE> mData; //!< 読み込んだデータ
    std::vector<D3D12_INPUT_ELEMENT_DESC> mLayout;
};

} //Utility 
} //Framework 