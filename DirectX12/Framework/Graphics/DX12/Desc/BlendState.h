#pragma once
#include <d3d12.h>

namespace Framework {
namespace Graphics {

/**
* @class BlendState
* @brief ブレンドステート
*/
class BlendState {
public:
    static inline D3D12_RENDER_TARGET_BLEND_DESC defaultBlendDesc() {
        D3D12_RENDER_TARGET_BLEND_DESC desc{};
        desc.BlendEnable = FALSE;
        desc.LogicOpEnable = FALSE;
        desc.SrcBlend = D3D12_BLEND::D3D12_BLEND_ONE;
        desc.DestBlend = D3D12_BLEND::D3D12_BLEND_ZERO;
        desc.BlendOp = D3D12_BLEND_OP::D3D12_BLEND_OP_ADD;
        desc.SrcBlendAlpha = D3D12_BLEND::D3D12_BLEND_ONE;
        desc.DestBlendAlpha = D3D12_BLEND::D3D12_BLEND_ZERO;
        desc.BlendOpAlpha = D3D12_BLEND_OP::D3D12_BLEND_OP_ADD;
        desc.LogicOp = D3D12_LOGIC_OP::D3D12_LOGIC_OP_NOOP;
        desc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE::D3D12_COLOR_WRITE_ENABLE_ALL;
        return desc;
    }
    static inline D3D12_RENDER_TARGET_BLEND_DESC addBlendDesc() {
        D3D12_RENDER_TARGET_BLEND_DESC desc;
        desc.BlendEnable = TRUE;
        desc.LogicOpEnable = FALSE;
        desc.SrcBlend = D3D12_BLEND::D3D12_BLEND_SRC_ALPHA;
        desc.DestBlend = D3D12_BLEND::D3D12_BLEND_ONE;
        desc.BlendOp = D3D12_BLEND_OP::D3D12_BLEND_OP_ADD;
        desc.SrcBlendAlpha = D3D12_BLEND::D3D12_BLEND_ONE;
        desc.DestBlendAlpha = D3D12_BLEND::D3D12_BLEND_ZERO;
        desc.BlendOpAlpha = D3D12_BLEND_OP::D3D12_BLEND_OP_ADD;
        desc.LogicOp = D3D12_LOGIC_OP::D3D12_LOGIC_OP_NOOP;
        desc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE::D3D12_COLOR_WRITE_ENABLE_ALL;
        return desc;
    }

    static inline D3D12_RENDER_TARGET_BLEND_DESC alignmentBlendDesc() {
        D3D12_RENDER_TARGET_BLEND_DESC desc;
        desc.BlendEnable = TRUE;
        desc.LogicOpEnable = FALSE;
        desc.SrcBlend = D3D12_BLEND::D3D12_BLEND_SRC_ALPHA;
        desc.DestBlend = D3D12_BLEND::D3D12_BLEND_INV_SRC_ALPHA;
        desc.BlendOp = D3D12_BLEND_OP::D3D12_BLEND_OP_ADD;
        desc.SrcBlendAlpha = D3D12_BLEND::D3D12_BLEND_ONE;
        desc.DestBlendAlpha = D3D12_BLEND::D3D12_BLEND_ZERO;
        desc.BlendOpAlpha = D3D12_BLEND_OP::D3D12_BLEND_OP_ADD;
        desc.LogicOp = D3D12_LOGIC_OP::D3D12_LOGIC_OP_NOOP;
        desc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE::D3D12_COLOR_WRITE_ENABLE_ALL;
        return desc;
    }
};

} //Graphics 
} //Framework 