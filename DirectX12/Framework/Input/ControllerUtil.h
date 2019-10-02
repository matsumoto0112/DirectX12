#pragma once
namespace Framework {
namespace Input {

/**
* @brief コントローラに関するユーティリティクラス
*/
class ControllerUtil {
public:
    static constexpr unsigned int AXIS_MAX_POSITION = 65535;; //!< 軸の最大座標
    static constexpr float HALF_AXIS_MAX_POSITION = AXIS_MAX_POSITION * 0.5f;; //!< 軸の最大座標の半分
};
} //Input 
} //Framework
