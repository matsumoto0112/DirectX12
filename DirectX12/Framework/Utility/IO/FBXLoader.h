#pragma once
#include <vector>
#include <string>
#include <fbxsdk.h>
#include "Framework/Math/Vector2.h"
#include "Framework/Math/Vector4.h"

namespace Framework {
namespace Utility {

/**
* @class FBXLoader
* @brief FBX�ǂݍ���
*/
class FBXLoader {
public:
    /**
    * @brief �R���X�g���N�^
    * @param filepath �t�@�C���ւ̃p�X
    * @param triangulate ���b�V�����O�p�`�ɂ��邩
    */
    FBXLoader(const std::string& filepath, bool triangulate = false);
    /**
    * @brief �f�X�g���N�^
    */
    ~FBXLoader();
    /**
    * @brief �ǂݍ��ݏ���
    * @param filepath �t�@�C���ւ̃p�X
    * @param triangulate ���b�V�����O�p�`�ɂ��邩
    */
    void load(const std::string& filepath, bool triangulate = false);

    std::vector<Math::Vector4> getPosition() const;

    bool hasUV() const;
    std::vector<Math::Vector2> getUV() const;
private:
    FbxManager* mManager; //!< FBX�Ǘ�
    FbxScene* mScene; //!< �ǂݍ��񂾃V�[��
};

} //Utility 
} //Framework 