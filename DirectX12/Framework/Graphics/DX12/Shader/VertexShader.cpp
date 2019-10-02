#include "VertexShader.h"
#include "Framework/Utility/Debug.h"

namespace Framework {
namespace Graphics {

VertexShader::VertexShader(const std::string& filename)
    :Shader(filename) { }

VertexShader::~VertexShader() { }

void VertexShader::getInputLayoutDesc(D3D12_GRAPHICS_PIPELINE_STATE_DESC* psoDesc) {
    //シェーダファイルの解析
    const int shaderSize = mShaderCode.size();
    BYTE* pInStruct = nullptr;
    for (unsigned long i = 0L; i < shaderSize - 4; i++) {
        if (memcmp(&mShaderCode[i], "ISGN", 4) == NULL) {
            pInStruct = &mShaderCode.at(i);
            break;
        }
    }

    if (pInStruct == nullptr) {
        MY_ASSERTION(false, "入力エレメントが解析できません");
    }

    //変数の数
    int cntVariable = pInStruct[8];
    std::vector<char*> semanticsName(cntVariable);
    std::vector<UINT> semanticsIndex(cntVariable);
    std::vector<DXGI_FORMAT> format(cntVariable);
    unsigned char* str = &pInStruct[16];
    for (int i = 0; i < cntVariable; i++) {
        semanticsName[i] = ((char*)(str[i * 24] + pInStruct + 8));
        semanticsIndex[i] = str[i * 24 + 4];
        //ベクトルの次元数
        const char dimension = str[i * 24 + 20];
        //型の種類
        const char variant = str[i * 24 + 12];
        switch (dimension) {
            //四次元
            case '\x0f':
                switch (variant) {
                    case D3D_REGISTER_COMPONENT_TYPE::D3D_REGISTER_COMPONENT_FLOAT32:
                        format[i] = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
                        break;
                    default:
                        format[i] = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
                        break;
                }
                break;
                //三次元
            case '\x07':
                switch (variant) {
                    case D3D_REGISTER_COMPONENT_TYPE::D3D_REGISTER_COMPONENT_FLOAT32:
                        format[i] = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
                        break;
                    default:
                        format[i] = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
                        break;
                }
                break;
                //二次元
            case '\x03':
                switch (variant) {
                    case D3D_REGISTER_COMPONENT_TYPE::D3D_REGISTER_COMPONENT_FLOAT32:
                        format[i] = DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT;
                        break;
                    default:
                        format[i] = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
                        break;
                }
                break;
                //一次元
            case '\x01':
                switch (variant) {
                    case D3D_REGISTER_COMPONENT_TYPE::D3D_REGISTER_COMPONENT_FLOAT32:
                        format[i] = DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
                        break;
                    case D3D_REGISTER_COMPONENT_TYPE::D3D_REGISTER_COMPONENT_UINT32:
                        format[i] = DXGI_FORMAT::DXGI_FORMAT_R32_UINT;
                        break;
                    default:
                        format[i] = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
                        break;
                }
                break;
            default:
                format[i] = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
                break;
        }
    }

    mInputElementDescs = std::vector< D3D12_INPUT_ELEMENT_DESC>(cntVariable);
    for (int i = 0; i < cntVariable; i++) {
        mInputElementDescs[i] = {
            semanticsName[i],
            semanticsIndex[i],
            format[i],
            0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            0 };
    }

    psoDesc->InputLayout.pInputElementDescs = mInputElementDescs.data();
    psoDesc->InputLayout.NumElements = mInputElementDescs.size();
}

} //Graphics 
} //Framework 
