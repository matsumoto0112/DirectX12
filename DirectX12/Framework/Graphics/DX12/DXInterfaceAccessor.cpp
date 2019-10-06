#include "DXInterfaceAccessor.h"
#include "Framework/Graphics/DX12/RenderingManager.h"

namespace Framework {
namespace Graphics {

ID3D12Device* DXInterfaceAccessor::getDevice() {
    return RenderingManager::getInstance().getDX12Manager()->getDevice();
}

} //Graphics 
} //Framework 
