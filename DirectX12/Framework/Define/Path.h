#pragma once

#include <string>
#include "Framework/Utility/Property.h"
#include "Framework/Utility/Singleton.h"

namespace Framework {
namespace Define {
/**
* @class Path
* @brief 各種ファイルへのパスを定義
*/
class Path : public Framework::Utility::Singleton<Path> {
public:
    Framework::Utility::GetterProperty<std::string> exe{ mExe }; //!< 実行ファイルまでのパス
    Framework::Utility::GetterProperty<std::string> shader{ mShader }; //!< シェーダファイルへのパス
    Framework::Utility::GetterProperty<std::string> resource{ mResource }; //!< リソースファイルへのパス
    Framework::Utility::GetterProperty<std::string> texture{ mTexture }; //!< テクスチャへのパス
    Framework::Utility::GetterProperty<std::string> fbx{ mFBX }; //!< FBXモデルへのパス
private:
    std::string mExe;
    std::string mShader;
    std::string mResource;
    std::string mTexture;
    std::string mFBX;
protected:
    /**
    * @brief コンストラクタ
    */
    Path();
    /**
    * @brief デストラクタ
    */
    ~Path();
};
} //Define 
} //Framework 
