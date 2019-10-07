#pragma once

#include <memory>
#include <unordered_map>
#include "Framework/Define/SceneType.h"

namespace Framework {
namespace Scene {
class SceneBase;
class Collecter;

/**
* @class Manager
* @brief シーン管理クラス
*/
class Manager {
private:
    using ScenePtr = std::unique_ptr<SceneBase>;
    using ScenePtrContainer = std::unordered_map<Define::SceneType, ScenePtr>;
public:
    /**
    * @brief コンストラクタ
    */
    Manager();
    /**
    * @brief デストラクタ
    */
    ~Manager();
    /**
    * @brief 更新
    */
    void update();
    /**
    * @brief 描画
    */
    void draw();
    /**
    * @brief シーンの登録
    * @param type シーンの種類
    * @param scene シーンデータ
    */
    void registerScene(Define::SceneType type, ScenePtr scene);
    /**
    * @brief シーンを読み込む
    */
    void loadScene(Define::SceneType next);
private:
    /**
    * @brief 次のシーンの取得
    * @param next 次のシーンの種類
    */
    SceneBase* getNextScene(Define::SceneType next);
private:
    SceneBase* mCurrentScene; //!< 現在のシーン
    ScenePtrContainer mCreatedScene; //!< 既に作られたシーンリスト
    std::unique_ptr<Collecter> mCollecter; //!< シーンコレクタ
};

} //Scene 
} //Framework 
