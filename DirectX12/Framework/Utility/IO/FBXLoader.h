#pragma once
#include <string>

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
    */
    FBXLoader(const std::string& name);
    /**
    * @brief �f�X�g���N�^
    */
    ~FBXLoader();

    void load(const std::string& name);
private:

};

} //Utility 
} //Framework 