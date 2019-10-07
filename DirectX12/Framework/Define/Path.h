#pragma once

#include <string>
#include "Framework/Utility/Property.h"
#include "Framework/Utility/Singleton.h"

namespace Framework {
namespace Define {
/**
* @class Path
* @brief �e��t�@�C���ւ̃p�X���`
*/
class Path : public Framework::Utility::Singleton<Path> {
public:
    Framework::Utility::GetterProperty<std::string> exe{ mExe }; //!< ���s�t�@�C���܂ł̃p�X
    Framework::Utility::GetterProperty<std::string> shader{ mShader }; //!< �V�F�[�_�t�@�C���ւ̃p�X
    Framework::Utility::GetterProperty<std::string> resource{ mResource }; //!< ���\�[�X�t�@�C���ւ̃p�X
    Framework::Utility::GetterProperty<std::string> texture{ mTexture }; //!< �e�N�X�`���ւ̃p�X
    Framework::Utility::GetterProperty<std::string> fbx{ mFBX }; //!< FBX���f���ւ̃p�X
private:
    std::string mExe;
    std::string mShader;
    std::string mResource;
    std::string mTexture;
    std::string mFBX;
protected:
    /**
    * @brief �R���X�g���N�^
    */
    Path();
    /**
    * @brief �f�X�g���N�^
    */
    ~Path();
};
} //Define 
} //Framework 
