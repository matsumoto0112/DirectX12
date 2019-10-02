#pragma once

#include "Framework/Math/Vector2.h"

namespace Framework {
namespace Device {
class GameDevice;
} //Device 

/**
* @class Game
* @brief ゲームクラス
*/
class Game {
protected:
    /**
    * @brief コンストラクタ
    * @param title ゲームのタイトル
    */
    Game(const std::string& title, HINSTANCE hInstance);
    /**
    * @brief デストラクタ
    */
    virtual ~Game();
public:
    /**
    * @brief 実行
    */
    int run();
    /**
    * @brief 初期化
    * @return 成功したらtrueを返す
    */
    virtual bool init();
    /**
    * @brief 終了処理
    */
    virtual void finalize();
protected:
    /**
    * @brief 更新
    */
    virtual void update();
    /**
    * @brief 描画
    */
    virtual void draw() = 0;
    /**
    * @brief 実行中か？
    */
    virtual bool isRunning();
private:
    //コピー禁止
    Game(const Game& other) = delete;
    Game& operator=(const Game& other) = delete;
};

} //Framework 