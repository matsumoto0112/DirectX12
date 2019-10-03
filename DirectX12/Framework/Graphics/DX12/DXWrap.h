#pragma once
#include <d3d12.h>
#include "Framework/Graphics/DX12/DX12Manager.h"

namespace Framework {
namespace Graphics {

/**
* @class Wrap
* @brief discription
*/
class Wrap {
public:
    /**
    * @brief �f�o�C�X���擾����
    */
    static inline ID3D12Device* getDevice() {
        return Framework::Graphics::DX12Manager::getInstance().getDevice();
    }
};


} //Graphics 
} //Framework 