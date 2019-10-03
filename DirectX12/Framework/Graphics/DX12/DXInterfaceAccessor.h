#pragma once
#include <d3d12.h>

namespace Framework {
namespace Graphics {

/**
* @class DXInterfaceAccessor
* @brief DirectX�C���^�[�t�F�[�X�ւ̃A�N�Z�T
*/
class DXInterfaceAccessor {
public:
    /**
    * @brief �f�o�C�X���擾����
    */
    static ID3D12Device* getDevice();
};

} //Graphics 
} //Framework 