#pragma once
#include <string>
#include <d3d12.h>
#include "Framework/Utility/Typedef.h"

namespace Framework {
namespace Graphics {

/**
* @class Texture
* @brief �e�N�X�`���N���X
*/
class Texture {
public:
    /**
    * @brief �R���X�g���N�^
    */
    Texture(const std::string& filepath);
    /**
    * @brief �f�X�g���N�^
    */
    ~Texture();
private:
    ComPtr<ID3D12DescriptorHeap> mSRVHeap; //!< �e�N�X�`��SRV�p
    ComPtr<ID3D12Resource> mTexture; //!< �e�N�X�`��
};

} //Graphics 
} //Framework 