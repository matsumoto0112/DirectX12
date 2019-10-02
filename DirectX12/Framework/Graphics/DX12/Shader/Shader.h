#pragma once
#include <vector>
#include <Windows.h>
#include <string>
#include <d3d12.h>

namespace Framework {
namespace Graphics {
/**
* @class Shader
* @brief シェーダークラス
*/
class Shader {
public:
    /**
    * @brief コンストラクタ
    */
    Shader(const std::string& filename);
    /**
    * @brief デストラクタ
    */
    virtual ~Shader();
    /**
    * @brief シェーダーデータを取得する
    */
    virtual D3D12_SHADER_BYTECODE getShaderData() const;
protected:
    std::vector<byte> mShaderCode; //!< シェーダーコード
};

} //Graphics 
} //Framework 