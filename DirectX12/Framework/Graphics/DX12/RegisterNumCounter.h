#pragma once
#include <d3d12.h>
#include "Framework/Utility/Property.h"

namespace Framework {
namespace Graphics {

/**
* @class RegisterNumCounter
* @brief レジスター番号カウンター
*/
class RegisterNumCounter {
public:
    /**
    * @brief コンストラクタ
    */
    RegisterNumCounter()
        :mNumber(0) { }
    /**
    * @brief デストラクタ
    */
    ~RegisterNumCounter() { };
    /**
    * @brief 利用可能な番号を取得する
    */
    UINT getNumber() {
        UINT res = mNumber;
        mNumber++;
        return res;
    }
private:
    UINT mNumber;
};

} //Graphics 
} //Framework 