#pragma once
#include <vector>
#include "Framework/Input/KeyCode.h"
#include "Framework/Utility/Property.h"
#include "Framework/Utility/Singleton.h"

namespace Framework {
namespace Define {

/**
* @class Config
* @brief ゲームの設定情報
*/
class Config : public Utility::Singleton<Config> {
public:
protected:
    /**
    * @brief コンストラクタ
    */
    Config();
    /**
    * @brief デストラクタ
    */
    ~Config();
private:
    int mScreenWidth; //!< スクリーン幅
    int mScreenHeight; //!< スクリーン高さ
    bool mKeepFPS; //!< FPSを保つようにするか
    float mFPS; //!< 目標FPS
    std::vector<Input::KeyCode> mGameEndKeys; //!< ゲームを終了するキー
public:
    Utility::GetterProperty<int> screenWidth{ mScreenWidth };
    Utility::GetterProperty<int> screenHeight{ mScreenHeight };
    Utility::GetterProperty<bool> keepFPS{ mKeepFPS };
    Utility::GetterProperty<float> FPS{ mFPS };
    Utility::GetRefProperty<std::vector<Input::KeyCode>> gameEndKeys{ mGameEndKeys };
};

} //Define 
} //Framework 