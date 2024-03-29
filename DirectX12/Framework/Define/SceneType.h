#pragma once

namespace Framework {
namespace Define {

/**
* @enum SceneType
* @brief シーンの種類定義
*/
enum class SceneType {
    ExecuteIndirect,
    RenderWhiteModel,
    RenderSamplingTexture,
    GPUParticle,
    ShadowMap,
};

} //Define 
} //Framework 