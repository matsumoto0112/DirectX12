//#include "Texture.h"
//#include "Framework/Utility/IO/TextureLoader.h"
//#include "Framework/Graphics/DX12/DXInterfaceAccessor.h"
//#include "Framework/Graphics/DX12/Helper.h"
//
//namespace Framework {
//namespace Graphics {
//Texture::Texture(const std::string& filepath) {
//
//    constexpr UINT TEXTURE_PIXEL_SIZE = 4;
//    UINT WIDTH = 256;
//    UINT HEIGHT = 256;
//    //テクスチャの生成
//    Framework::Utility::TextureLoader loader;
//    std::vector<BYTE> data = loader.load(filepath, &WIDTH, &HEIGHT);
//
//    D3D12_RESOURCE_DESC desc{};
//    desc.MipLevels = 1;
//    desc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
//    desc.Width = WIDTH;
//    desc.Height = HEIGHT;
//    desc.Flags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE;
//    desc.DepthOrArraySize = 1;
//    desc.SampleDesc.Count = 1;
//    desc.SampleDesc.Quality = 0;
//    desc.Dimension = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D;
//
//    throwIfFailed(DXInterfaceAccessor::getDevice()->CreateCommittedResource(
//        &createProperty(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT),
//        D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
//        &desc,
//        D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST,
//        nullptr,
//        IID_PPV_ARGS(&mTexture)));
//
//    const UINT64 uploadBufferSize = getRequiredIntermediateSize(mTexture.Get(), 0, 1);
//
//    ComPtr<ID3D12Resource> textureUploadHeap;
//    //GPUアップロードバッファ作成
//    throwIfFailed(mDevice->CreateCommittedResource(
//        &PROPERTY(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD),
//        D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
//        &RESOURCE(uploadBufferSize),
//        D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
//        nullptr,
//        IID_PPV_ARGS(&textureUploadHeap)));
//
//    std::vector<UINT8> texture = data;
//
//    D3D12_SUBRESOURCE_DATA sub{};
//    sub.pData = &texture[0];
//    sub.RowPitch = WIDTH * TEXTURE_PIXEL_SIZE;
//    sub.SlicePitch = sub.RowPitch * HEIGHT;
//    updateSubresource(Framework::Graphics::DX12Manager::getInstance().getCommandList(), mTexture.Get(), textureUploadHeap.Get(), 0, 0, 1, &sub);
//
//    Framework::Graphics::DX12Manager::getInstance().getCommandList()->ResourceBarrier(1, &BARRIER(mTexture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
//
//
//    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
//    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
//    srvDesc.Format = desc.Format;
//    srvDesc.ViewDimension = D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2D;
//    srvDesc.Texture2D.MipLevels = 1;
//    mDevice->CreateShaderResourceView(mTexture.Get(), &srvDesc, mSRVHeap->GetCPUDescriptorHandleForHeapStart());
//}
//
//} //Graphics 
//} //Framework 
