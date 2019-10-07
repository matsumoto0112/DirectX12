#pragma once
#include "Framework/Scene/SceneBase.h"

/**
* @class ExecuteIndirect
* @brief discription
*/
class ExecuteIndirect : public Framework::Scene::SceneBase {
public:
    /**
    * @brief コンストラクタ
    */
    ExecuteIndirect();
    /**
    * @brief デストラクタ
    */
    ~ExecuteIndirect();
    /**
    * @brief シーンデータの読み込み
    */
    virtual void load(Framework::Scene::Collecter& collecter) override;
    /**
    * @brief 更新
    */
    virtual void update() override;
    /**
    * @brief 終了しているか
    */
    virtual bool isEndScene() const override;
    /**
    * @brief 描画
    */
    virtual void draw() override;
    /**
    * @brief 終了処理
    */
    virtual void end() override;
    /**
    * @brief 次のシーン
    */
    virtual Framework::Define::SceneType next() override;
private:
};
