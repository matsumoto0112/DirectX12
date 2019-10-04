#pragma once
#include <d3d12.h>
#include "Framework/Utility/Debug.h"

namespace Framework {
namespace Graphics {

/**
* @enum VisibilityType
* @brief �V�F�[�_�[����̉���Ԏ�ޒ�`
*/
enum class VisibilityType {
    All,
    Vertex,
    Pixel,
    Geometory,
};

inline constexpr D3D12_SHADER_VISIBILITY convertToD3D12_SHADER_VISIBILITY(Framework::Graphics::VisibilityType visibility) {
    switch (visibility) {
        case Framework::Graphics::VisibilityType::All:
            return D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL;
        case Framework::Graphics::VisibilityType::Vertex:
            return D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_VERTEX;
        case Framework::Graphics::VisibilityType::Pixel:
            return D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_PIXEL;
        case Framework::Graphics::VisibilityType::Geometory:
            return D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_GEOMETRY;
        default:
            MY_ASSERTION(false, "VisibilityType��D3D12_SHADER_VISIBILITY�ɕϊ��ł��܂���");
            return D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL;
    }
}


} //Graphics 
} //Framework 