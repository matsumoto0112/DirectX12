#pragma once
#include <vector>
#include <memory>
#include <d3d12.h>
#include "Framework/Graphics/DX12/Resource/Texture.h"

namespace Framework {
namespace Graphics {
class ShaderResourceView;

/**
* @class SRVManager
* @brief �V�F�[�_�[���\�[�X�r���[�Ǘ�
*/
class SRVManager {
public:
    /**
    * @brief �R���X�g���N�^
    */
    SRVManager();
    /**
    * @brief �f�X�g���N�^
    */
    ~SRVManager();

    void beginFrame();

    void beginUpdate();

    void endUpdate();

    void setResource();
private:
    static constexpr UINT TEXTURE_RESERVATION_NUM = 10000;
    std::vector<std::unique_ptr<ShaderResourceView>> mSRVs;
};

} //Graphics 
} //Framework 