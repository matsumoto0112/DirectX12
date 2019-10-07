#pragma once
#include <vector>
#include <string>
#include <fbxsdk.h>
#include "Framework/Math/Vector4.h"

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
    * @param filepath ファイルへのパス
    * @param triangulate メッシュを三角形にするか
    */
    FBXLoader(const std::string& filepath, bool triangulate = false);
    /**
    * @brief デストラクタ
    */
    ~FBXLoader();
    /**
    * @brief 読み込み処理
    * @param filepath ファイルへのパス
    * @param triangulate メッシュを三角形にするか
    */
    void load(const std::string& filepath, bool triangulate = false);

    std::vector<Math::Vector4> getPosition() const;
private:
    FbxManager* mManager; //!< FBX管理
    FbxScene* mScene; //!< 読み込んだシーン
};

} //Utility 
} //Framework 