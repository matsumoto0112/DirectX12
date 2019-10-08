#pragma once

namespace Framework {
namespace Define {

/**
* @class Debug
* @brief デバッグ情報定義
*/
class Debug {
    static constexpr bool ENABLE_GPU_DEBUG = false; //!< GPUデバッグを有効にするか
    static constexpr bool USE_WRAP_DEVICE = true;
};

} //Define 
} //Framework 