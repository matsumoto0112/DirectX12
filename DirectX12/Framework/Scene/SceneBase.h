#pragma once

#include <memory>
#include "Framework/Define/SceneType.h"

namespace Framework {
namespace Scene {
class Collecter;

/**
* @class SceneBase
* @brief シーンインターフェース
*/
class SceneBase {
public:
    /**
    * @brief デストラクタ
    */
    virtual ~SceneBase() = default;
    /**
    * @brief シーンのデータ読み込み
    * @param collecter シーンデータコレクタ
    */
    virtual void load(Collecter& collecter) = 0;
    /**
    * @brief 更新
    */
    virtual void update() = 0;
    /**
    * @brief シーンが終了しているか
    */
    virtual bool isEndScene() const = 0;
    /**
    * @brief 描画
    */
    virtual void draw() = 0;
    /**
    * @brief シーンの終了処理
    */
    virtual void end() = 0;
    /**
    * @brief 次のシーン
    */
    virtual Define::SceneType next() = 0;
};

} //Scene 
} //Framework 
