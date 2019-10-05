#pragma once
#include <d3d12.h>

namespace Framework {
namespace Graphics {

/**
* @enum FillMode
* @brief 塗りつぶしモード
*/
enum class FillMode {
    Solid,
    WireFrame,
};

/**
* @enum CullMode
* @brief カリングモード
*/
enum class CullMode {
    Back,
    Front,
    None,
};

/**
* @class Rasterizer
* @brief ラスタライザ設定
*/
struct Rasterizer : public D3D12_RASTERIZER_DESC {
public:
    explicit Rasterizer(Framework::Graphics::FillMode fillMode, Framework::Graphics::CullMode cullMode) {
        FillMode = toD3D12_FILL_MODE(fillMode);
        CullMode = toD3D12_CULL_MODE(cullMode);
        FrontCounterClockwise = FALSE;
        DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
        DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
        DepthClipEnable = TRUE;
        MultisampleEnable = FALSE;
        AntialiasedLineEnable = FALSE;
        ForcedSampleCount = 0;
        ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE::D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
    }
private:
    static inline D3D12_FILL_MODE toD3D12_FILL_MODE(Framework::Graphics::FillMode fill) {
        switch (fill) {
            case Framework::Graphics::FillMode::Solid:
                return D3D12_FILL_MODE::D3D12_FILL_MODE_SOLID;
            default:
                return D3D12_FILL_MODE::D3D12_FILL_MODE_WIREFRAME;
        }
    }
    static inline D3D12_CULL_MODE toD3D12_CULL_MODE(Framework::Graphics::CullMode cull) {
        switch (cull) {
            case Framework::Graphics::CullMode::Back:
                return D3D12_CULL_MODE::D3D12_CULL_MODE_BACK;
            case Framework::Graphics::CullMode::Front:
                return D3D12_CULL_MODE::D3D12_CULL_MODE_FRONT;
            default:
                return D3D12_CULL_MODE::D3D12_CULL_MODE_NONE;
        }
    }
};

} //Graphics 
} //Framework 