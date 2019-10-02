#pragma once

#include <list>
#include <Windows.h>

namespace Framework {
namespace Utility {

/**
* @class FPS
* @brief FPS計測クラス
* @detail 参考:http://marupeke296.com/DXCLSSmp_FPSCounter.html
*/
class FPS {
public:
    /**
    * @brief コンストラクタ
    * @param sample サンプル数(多いほど全体の平均が求まるがメモリを多く使う)
    */
    FPS(UINT sample = 10);
    /**
    * @brief デストラクタ
    */
    ~FPS();
    /**
    * @brief FPS計測
    * @return FPS(ミリ秒)
    */
    double getFPS();
    /**
    * @brief サンプル数の設定
    * @param sample サンプル数(多いほど全体の平均が求まるがメモリを多く使う)
    */
    void setSample(UINT sample);
private:
    /**
    * @brief 現在の差分時間（ミリ秒）
    */
    double getCurrentDifferenceTime();
    /**
    * @brief FPSの更新
    * @param diff 差分時間
    */
    double updateFPS(double diff);
private:
    double mFreq; //!< 環境の周波数
    LARGE_INTEGER mCounter; //!< クロックカウント数
    LONGLONG mOldCounter; //!< 以前のクロックカウント数
    double mSumTimes; //!< 共通部分の平均値
    UINT mSample; //!< サンプル数
    std::list<double> mDifferenceTimeList; //!< 経過時間のリスト
};

} //FPS
} //Framework 
