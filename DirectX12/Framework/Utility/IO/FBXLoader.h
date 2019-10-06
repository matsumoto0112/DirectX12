#pragma once
#include <string>

namespace Framework {
namespace Utility {

/**
* @class FBXLoader
* @brief FBX読み込み
*/
class FBXLoader {
public:
    /**
    * @brief コンストラクタ
    */
    FBXLoader(const std::string& name);
    /**
    * @brief デストラクタ
    */
    ~FBXLoader();

    void load(const std::string& name);
private:

};

} //Utility 
} //Framework 