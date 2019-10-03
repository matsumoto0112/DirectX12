#include "DXInterfaceAccessor.h"
#include "Framework/Graphics/DX12/DX12Manager.h"

namespace Framework {
namespace Graphics {

ID3D12Device* DXInterfaceAccessor::getDevice() {
    return DX12Manager::getInstance().getDevice();
}

} //Graphics 
} //Framework 
