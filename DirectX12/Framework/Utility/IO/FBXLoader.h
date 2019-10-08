#pragma once
#include <vector>
#include <string>
#include <fbxsdk.h>
#include "Framework/Math/Vector2.h"
#include "Framework/Math/Vector3.h"
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
    /**
    * @brief 頂点座標の取得
    * @details 複数メッシュの頂点をすべて取得する
    */
    std::vector<Math::Vector4> getPosition() const;
    /**
    * @brief オブジェクトがUVを持っているか
    */
    bool hasUV() const;
    /**
    * @brief UV座標の取得
    * @details 複数メッシュのすべてのUV座標を取得する
    */
    std::vector<Math::Vector2> getUV() const;
    /**
    * @brief 法線情報を持っているか
    */
    bool hasNormal() const;
    /**
    * @brief 法線情報の取得
    * @details 複数メッシュのすべての法線情報を取得する
    */
    std::vector<Math::Vector3> getNormal() const;
private:
    FbxManager* mManager; //!< FBX管理
    FbxScene* mScene; //!< 読み込んだシーン
};

} //Utility 
} //Framework 