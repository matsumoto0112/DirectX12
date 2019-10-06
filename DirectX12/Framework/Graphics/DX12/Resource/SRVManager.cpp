#include "SRVManager.h"
#include "Framework/Graphics/DX12/DXInterfaceAccessor.h"
#include "Framework/Graphics/DX12/Helper.h"

namespace Framework {
namespace Graphics {

SRVManager::SRVManager() {
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
    srvHeapDesc.NumDescriptors =TEXTURE_RESERVATION_NUM;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    throwIfFailed(DXInterfaceAccessor::getDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&mSRVHeap)));
}

} //Graphics 
} //Framework 
