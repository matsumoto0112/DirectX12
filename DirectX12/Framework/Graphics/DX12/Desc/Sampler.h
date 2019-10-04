#pragma once
#include <d3d12.h>
#include "Framework/Graphics/DX12/VisibilityType.h"

namespace Framework {
namespace Graphics {

/**
* @enum FilterMode
* @brief �t�B���^�[�̎��
*/
enum class FilterMode {
    Linear,
    Point,
};

/**
* @enum AddressMode
* @brief �e�N�X�`���̃A�h���X���[�h
*/
enum class AddressMode {
    Wrap,
    Border,
    Clamp,
    Mirror,
    MirrorOnce,
};

/**
* @class Sampler
* @brief discription
*/
class Sampler {
public:
    /**
    * @brief �R���X�g���N�^
    */
    static constexpr D3D12_STATIC_SAMPLER_DESC createStaticSampler(
        FilterMode filter,
        AddressMode address,
        VisibilityType visibility,
        UINT registerNum) {
        D3D12_STATIC_SAMPLER_DESC sampler{};
        sampler.Filter = toD3D12_FILTER(filter);
        sampler.AddressU = toD3D12_TEXTURE_ADDRESS_MODE(address);
        sampler.AddressV = toD3D12_TEXTURE_ADDRESS_MODE(address);
        sampler.AddressW = toD3D12_TEXTURE_ADDRESS_MODE(address);
        sampler.MipLODBias = 0;
        sampler.MaxAnisotropy = 0;
        sampler.MinLOD = 0.0f;
        sampler.MaxLOD = D3D12_FLOAT32_MAX;
        sampler.ShaderRegister = registerNum;
        sampler.RegisterSpace = 0;
        sampler.ShaderVisibility = convertToD3D12_SHADER_VISIBILITY(visibility);

        return sampler;
    }
private:
    static constexpr D3D12_TEXTURE_ADDRESS_MODE toD3D12_TEXTURE_ADDRESS_MODE(AddressMode address) {
        switch (address) {
            case Framework::Graphics::AddressMode::Wrap:
                return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_WRAP;
            case Framework::Graphics::AddressMode::Border:
                return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_BORDER;
            case Framework::Graphics::AddressMode::Clamp:
                return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
            case Framework::Graphics::AddressMode::Mirror:
                return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
            default:
                return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
        }
    }
    static constexpr D3D12_FILTER toD3D12_FILTER(FilterMode filter) {
        switch (filter) {
            case Framework::Graphics::FilterMode::Linear:
                return D3D12_FILTER::D3D12_FILTER_MIN_MAG_MIP_LINEAR;
            case Framework::Graphics::FilterMode::Point:
                return D3D12_FILTER::D3D12_FILTER_MIN_MAG_MIP_POINT;
            default:
                MY_ASSERTION(false, "filter��D3D12_FILTER�ɕϊ��ł��܂���ł���");
                return D3D12_FILTER::D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
        }
    }
};

} //Graphics 
} //Framework 