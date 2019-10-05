#include "Texture.h"
#include "Framework/Utility/IO/TextureLoader.h"
#include "Framework/Graphics/DX12/DXInterfaceAccessor.h"
#include "Framework/Graphics/DX12/Helper.h"

namespace Framework {
namespace Graphics {

Texture::Texture(const std::string& filepath) {
    UINT WIDTH;
    UINT HEIGHT;
    //テクスチャの生成
    Framework::Utility::TextureLoader loader;
    std::vector<BYTE> data = loader.load(filepath, &WIDTH, &HEIGHT);

    D3D12_HEAP_PROPERTIES heapPropertices{};
    heapPropertices.Type = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_CUSTOM;
    heapPropertices.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
    heapPropertices.MemoryPoolPreference = D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_L0;
    heapPropertices.VisibleNodeMask = 1;
    heapPropertices.CreationNodeMask = 1;

    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    resourceDesc.Width = WIDTH;
    resourceDesc.Height = HEIGHT;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_UNKNOWN;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.SampleDesc.Quality = 0;
    throwIfFailed(DXInterfaceAccessor::getDevice()->CreateCommittedResource(&heapPropertices, D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
        &resourceDesc, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&mTexture)));

    D3D12_BOX box = { 0,0,0,(UINT)WIDTH,(UINT)HEIGHT,1 };
    throwIfFailed(mTexture->WriteToSubresource(0, &box, data.data(), WIDTH * TEXTURE_PIXEL_SIZE, WIDTH * HEIGHT * TEXTURE_PIXEL_SIZE));

    //D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
    //descriptorHeapDesc.NumDescriptors = 1;
    //descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    //descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    //descriptorHeapDesc.NodeMask = 0;
    //throwIfFailed(DXInterfaceAccessor::getDevice()->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&mTextureDH)));

    //D3D12_CPU_DESCRIPTOR_HANDLE handleSRV{};
    //D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    //srvDesc.Format = resourceDesc.Format;
    //srvDesc.ViewDimension = D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2D;
    //srvDesc.Texture2D.MipLevels = 1;
    //srvDesc.Texture2D.MostDetailedMip = 0;
    //srvDesc.Texture2D.PlaneSlice = 0;
    //srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
    //srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    ////handleSRV = mTextureDH->GetCPUDescriptorHandleForHeapStart();
    //DXInterfaceAccessor::getDevice()->CreateShaderResourceView(mTexture.Get(), &srvDesc,);

}

Texture::~Texture() { }

void Texture::addToCommandList(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex) {
    //commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, mTextureDH->GetGPUDescriptorHandleForHeapStart());
}

} //Graphics 
} //Framework 
