#include "FPS.h"
#include <Windows.h>

namespace Framework {
namespace Utility {

FPS::FPS(UINT sample) {
    setSample(sample);
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&mCounter);
    mOldCounter = mCounter.QuadPart;
    mFreq = (double)freq.QuadPart;
    getFPS();
}

FPS::~FPS() {
    mDifferenceTimeList.clear();
}

double FPS::getFPS() {
    double diff = getCurrentDifferenceTime();
    //差分が0だと計算できない
    if (diff == 0) {
        return 0;
    }
    return updateFPS(diff);

}

void FPS::setSample(UINT sample) {
    mSample = sample;
    mDifferenceTimeList.resize(sample, 0.0f);
    mSumTimes = 0;
}

double FPS::getCurrentDifferenceTime() {
    QueryPerformanceCounter(&mCounter);
    double diff = (double)(mCounter.QuadPart - mOldCounter); //差分
    mOldCounter = mCounter.QuadPart; //値の更新
    return diff * 1000 / mFreq;
}

double FPS::updateFPS(double diff) {
    //古いデータを削除
    mDifferenceTimeList.pop_front();
    //新しいデータを追加
    mDifferenceTimeList.push_back(diff);
    //平均値は(共通部分の合計+差分)/サンプル数
    double average = (mSumTimes + diff) / mSample;
    double FPS = 0;
    if (average != 0) {
        FPS = 1000.0 / average;
    }
    //共通部分の更新
    //新しい値を足して古い値を引く
    mSumTimes += diff - *mDifferenceTimeList.begin();
    return FPS;
}


} //Utility
} //Framework 
