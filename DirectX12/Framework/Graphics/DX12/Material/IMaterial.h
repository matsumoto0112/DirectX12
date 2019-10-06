#pragma once
#include <d3d12.h>

namespace Framework {
namespace Graphics {

/**
* @class IMaterial
* @brief �}�e���A�����ۃN���X
*/
class IMaterial {
public:
    /**
    * @brief �f�X�g���N�^
    */
    virtual ~IMaterial() = default;
    /**
    * @brief �R�}���h���X�g�ɒǉ�����
    */
    virtual void addToCommandList(ID3D12GraphicsCommandList* commandList) = 0;
};

} //Graphics 
} //Framework 