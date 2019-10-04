#include "ShaderReader.h"
#include "Framework/Utility/Debug.h"
#include "Framework/Utility/IO/ByteReader.h"

namespace Framework {
namespace Utility {

ShaderReader::ShaderReader(const std::string& filepath) :
    mLayout(0) {
    Framework::Utility::ByteReader reader(filepath);
    mData = reader.get();
}

ShaderReader::~ShaderReader() { }

std::vector<BYTE> ShaderReader::get() const {
    return mData;
}

std::vector<D3D12_INPUT_ELEMENT_DESC> ShaderReader::getShaderReflection() {
    if (mLayout.size() != 0)return mLayout;
    const UINT shaderSize = mData.size();

    //シェーダファイルの解析
    BYTE* pInStruct = nullptr;
    for (unsigned long i = 0L; i < shaderSize - 4; i++) {
        if (memcmp(&mData[i], "ISGN", 4) == NULL) {
            pInStruct = &mData.at(i);
            break;
        }
    }
    if (pInStruct == nullptr) {
        MY_ASSERTION(false, "入力エレメントが解析できません");
    }

    //変数の数
    int cntVariable = pInStruct[8];
    std::vector<char*>semanticsName(cntVariable);
    std::vector<UINT> semanticsIndex(cntVariable);
    std::vector<DXGI_FORMAT> format(cntVariable);
    unsigned char* str = &pInStruct[16];
    for (int i = 0; i < cntVariable; i++) {
        semanticsName[i] = (char*)(str[i * 24] + pInStruct + 8);
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

    mLayout = std::vector<D3D12_INPUT_ELEMENT_DESC>(cntVariable);
    for (int i = 0; i < cntVariable; i++) {
        mLayout[i] = {
            semanticsName[i],
            semanticsIndex[i],
            format[i],
            0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            0 };
    }

    return mLayout;
}

} //Utility 
} //Framework 


