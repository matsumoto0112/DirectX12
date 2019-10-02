#include "Shader.h"
#include "Framework/Utility/IO/ByteReader.h"
#include "Framework/Utility/Debug.h"

namespace Framework {
namespace Graphics {

Shader::Shader(const std::string& filename) {
    Utility::ByteReader reader(filename);
    mShaderCode = reader.get();
    MY_ERROR_WINDOW(!mShaderCode.empty(), "シェーダーリソースの読み込みに失敗しました");
}
Shader::~Shader() { }

D3D12_SHADER_BYTECODE Shader::getShaderData() const {
    D3D12_SHADER_BYTECODE result{};
    result.pShaderBytecode = mShaderCode.data();
    result.BytecodeLength = mShaderCode.size();
    return result;
}

} //Graphics 
} //Framework 
