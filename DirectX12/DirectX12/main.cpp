//メモリリーク検出
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h> 
#include <crtdbg.h>  

#include <Windows.h>
#include <tchar.h>
#include <d3d12.h>
#include <dxgi1_4.h>

#pragma comment(linker,"/SUBSYSTEM:WINDOWS")

#include "Framework/Define/Config.h"
#include "Framework/Define/Path.h"
#include "Framework/Device/GameDevice.h"
#include "Framework/Game.h"
#include "Framework/Graphics/Color4.h"
#include "Framework/Graphics/DX12/DX12Manager.h"
#include "Framework/Math/Vector3.h"
#include "Framework/Math/Vector4.h"
#include "Framework/Utility/IO/ByteReader.h"
#include "Framework/Window/Window.h"
#include "Framework/Window/Procedure/DestroyProc.h"
#include "Framework/Window/Procedure/CloseProc.h"
#include "Framework/Utility/IO/TextureLoader.h"
#include "Framework/Math/Matrix4x4.h"
#include "Framework/Graphics/DX12/Buffer/VertexBuffer.h"
#include "Framework/Graphics/DX12/Buffer/IndexBuffer.h"
#include "Framework/Graphics/DX12/Helper.h"

namespace {
using namespace Framework::Graphics;
struct Vertex {
    Framework::Math::Vector4 pos;
    Framework::Math::Vector2 uv;
};

struct ColorBuffer {
    Framework::Graphics::Color4 color;
};

struct MVP {
    Framework::Math::Matrix4x4 world;
    Framework::Math::Matrix4x4 view;
    Framework::Math::Matrix4x4 proj;
};

void getHardwareAdapter(IDXGIFactory2* factory, IDXGIAdapter1** ppAdapter) {
    ComPtr<IDXGIAdapter1> adapter = nullptr;
    *ppAdapter = nullptr;

    for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(adapterIndex, &adapter); adapterIndex++) {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);

        if (desc.Flags & DXGI_ADAPTER_FLAG::DXGI_ADAPTER_FLAG_SOFTWARE) {
            continue;
        }

        if (SUCCEEDED(D3D12CreateDevice(
            adapter.Get(),
            D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0,
            __uuidof(ID3D12Device),
            nullptr))) {
            break;
        }
    }

    *ppAdapter = adapter.Detach();
}

D3D12_HEAP_PROPERTIES PROPERTY(D3D12_HEAP_TYPE type) {
    D3D12_HEAP_PROPERTIES prop{};
    prop.Type = type;
    prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    prop.MemoryPoolPreference = D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_UNKNOWN;
    prop.CreationNodeMask = 1;
    prop.VisibleNodeMask = 1;
    return prop;
}

D3D12_RESOURCE_DESC RESOURCE(UINT size) {
    D3D12_RESOURCE_DESC res{};
    res.Dimension = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER;
    res.Alignment = 0;
    res.Width = size;
    res.Height = 1;
    res.DepthOrArraySize = 1;
    res.MipLevels = 1;
    res.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
    res.SampleDesc.Count = 1;
    res.SampleDesc.Quality = 0;
    res.Layout = D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    res.Flags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE;
    return res;
}

D3D12_RESOURCE_BARRIER BARRIER(ID3D12Resource* resource,
    D3D12_RESOURCE_STATES before,
    D3D12_RESOURCE_STATES after,
    UINT subResource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
    D3D12_RESOURCE_BARRIER_FLAGS flag = D3D12_RESOURCE_BARRIER_FLAGS::D3D12_RESOURCE_BARRIER_FLAG_NONE) {
    D3D12_RESOURCE_BARRIER result{};
    result.Type = D3D12_RESOURCE_BARRIER_TYPE::D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    result.Flags = flag;
    result.Transition.pResource = resource;
    result.Transition.StateBefore = before;
    result.Transition.StateAfter = after;
    result.Transition.Subresource = subResource;
    return result;
}

UINT64 getRequiredIntermediateSize(ID3D12Resource* resource, UINT firstSubresource, UINT numSubresource) {
    D3D12_RESOURCE_DESC desc = resource->GetDesc();
    UINT64 requiredSize = 0;

    ID3D12Device* device = nullptr;
    resource->GetDevice(__uuidof(*device), reinterpret_cast<void**>(&device));
    device->GetCopyableFootprints(&desc, firstSubresource, numSubresource, 0, nullptr, nullptr, nullptr, &requiredSize);
    device->Release();

    return requiredSize;
}

std::vector<UINT8> generate(UINT width, UINT height, UINT texPixelSize) {
    const UINT textureSize = width * height * texPixelSize;
    std::vector<UINT8> data(textureSize);
    for (UINT n = 0; n < textureSize; n += texPixelSize) {
        data[n + 0] = 0.0f;
        data[n + 1] = Framework::Math::MathUtil::lerp(0, 0xff, static_cast<float>(n) / static_cast<float>(textureSize));
        data[n + 2] = 0x00;
        data[n + 3] = 0xff;
    }
    return data;
}

void memcpySubResource(
    const D3D12_MEMCPY_DEST* dest,
    const D3D12_SUBRESOURCE_DATA* src,
    SIZE_T rowSizeInBytes,
    UINT numRows,
    UINT numSlices) {
    for (UINT z = 0; z < numSlices; z++) {
        BYTE* destSlice = reinterpret_cast<BYTE*>(dest->pData) + dest->SlicePitch * z;
        const BYTE* srcSlice = reinterpret_cast<const BYTE*>(src->pData) + src->SlicePitch * z;
        for (UINT y = 0; y < numRows; y++) {
            memcpy(destSlice + dest->RowPitch * y,
                srcSlice + src->RowPitch * y,
                rowSizeInBytes);
        }
    }
}

UINT64 updateSubresource(ID3D12GraphicsCommandList* list,
    ID3D12Resource* resource,
    ID3D12Resource* intermediate,
    UINT firstSubresource,
    UINT numSubresource,
    UINT requiredSize,
    const D3D12_PLACED_SUBRESOURCE_FOOTPRINT* layouts,
    const UINT* numRows,
    const UINT64* rowSizesInBytes,
    const D3D12_SUBRESOURCE_DATA* srcData) {
    D3D12_RESOURCE_DESC imdDesc = intermediate->GetDesc();
    D3D12_RESOURCE_DESC dstDesc = resource->GetDesc();
    if (imdDesc.Dimension != D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER ||
        imdDesc.Width < requiredSize + layouts[0].Offset ||
        requiredSize > SIZE_T(-1) ||
        (dstDesc.Dimension == D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER &&
        (firstSubresource != 0 || numSubresource != 1))) {
        return 0;
    }

    BYTE* data;
    HRESULT hr = intermediate->Map(0, nullptr, reinterpret_cast<void**>(&data));
    if (FAILED(hr))return 0;

    for (UINT i = 0; i < numSubresource; i++) {
        if (rowSizesInBytes[i] > SIZE_T(-1))return 0;
        D3D12_MEMCPY_DEST destData = { data + layouts[i].Offset ,layouts[i].Footprint.RowPitch,SIZE_T(layouts[i].Footprint.RowPitch) * SIZE_T(numRows[i]) };
        memcpySubResource(&destData, &srcData[i], static_cast<SIZE_T>(rowSizesInBytes[i]), numRows[i], layouts[i].Footprint.Depth);
    }

    intermediate->Unmap(0, nullptr);

    if (dstDesc.Dimension == D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER) {
        list->CopyBufferRegion(
            resource, 0, intermediate, layouts[0].Offset, layouts[0].Footprint.Width);
    }
    else {
        for (UINT i = 0; i < numSubresource; i++) {
            D3D12_TEXTURE_COPY_LOCATION dst{};
            dst.pResource = resource;
            dst.Type = D3D12_TEXTURE_COPY_TYPE::D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
            dst.SubresourceIndex = i + firstSubresource;
            D3D12_TEXTURE_COPY_LOCATION src{};
            src.pResource = intermediate;
            src.Type = D3D12_TEXTURE_COPY_TYPE::D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
            src.PlacedFootprint = layouts[i];
            list->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
        }
    }
    return requiredSize;
}

UINT64 updateSubresource(ID3D12GraphicsCommandList* list,
    ID3D12Resource* resource,
    ID3D12Resource* intermediate,
    UINT intermediateOffset,
    UINT firstSubresource,
    UINT numSubresources,
    D3D12_SUBRESOURCE_DATA* srcData) {
    UINT64 requiredSize = 0;
    UINT64 memToAlloc = static_cast<UINT64>(sizeof(D3D12_PLACED_SUBRESOURCE_FOOTPRINT) + sizeof(UINT) + sizeof(UINT64))* numSubresources;
    if (memToAlloc > SIZE_MAX) {
        return 0;
    }

    void* mem = HeapAlloc(GetProcessHeap(), 0, static_cast<SIZE_T>(memToAlloc));
    if (mem == nullptr)return 0;

    D3D12_PLACED_SUBRESOURCE_FOOTPRINT* layouts = reinterpret_cast<D3D12_PLACED_SUBRESOURCE_FOOTPRINT*>(mem);
    UINT64* rowSizesInBytes = reinterpret_cast<UINT64*>(layouts + numSubresources);
    UINT* numRows = reinterpret_cast<UINT*>(rowSizesInBytes + numSubresources);

    D3D12_RESOURCE_DESC desc = resource->GetDesc();
    ID3D12Device* device;
    resource->GetDevice(__uuidof(*device), reinterpret_cast<void**>(&device));
    device->GetCopyableFootprints(&desc, firstSubresource, numSubresources, intermediateOffset, layouts, numRows, rowSizesInBytes, &requiredSize);
    device->Release();
    UINT64 res = updateSubresource(list, resource, intermediate, firstSubresource, numSubresources, requiredSize, layouts, numRows, rowSizesInBytes, srcData);
    HeapFree(GetProcessHeap(), 0, mem);
    return res;
}
}

/**
* @class MyGame
*/
class MyGame :public Framework::Game {
public:
    /**
    * @brief コンストラクタ
    */
    MyGame(const std::string& title, HINSTANCE hInstance) :Framework::Game(title, hInstance) { }
    /**
    * @brief デストラクタ
    */
    ~MyGame() { }
    virtual bool init() override {
        using namespace Framework::Graphics;
        if (!Game::init())return false;
        using namespace Framework::Window;
        Window* window = Framework::Device::GameDevice::getInstance().getWindow();
        window->addProcedureEvent(new DestroyProc());
        window->addProcedureEvent(new CloseProc());

        mColorBuffer.color = Framework::Graphics::Color4(1.0f, 0.0f, 0.0f, 1.0f);

        UINT width = Framework::Define::Config::getInstance().screenWidth;
        UINT height = Framework::Define::Config::getInstance().screenHeight;
        Framework::Graphics::DX12Manager::getInstance().initialize(window->getHWND(), width, height);
        ID3D12Device* mDevice = Framework::Graphics::DX12Manager::getInstance().getDevice();
        {
            //SRV
            D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
            srvHeapDesc.NumDescriptors = 1;
            srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            throwIfFailed(mDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&mSRVHeap)));

            //CBV
            D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc{};
            cbvHeapDesc.NumDescriptors = 1;
            cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            throwIfFailed(mDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&mCBVHeap)));

            throwIfFailed(mDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&mMatrixCBVHeap)));
        }

        //頂点バッファ作成
        {
            std::vector<Vertex> vertices{
                 {{-1.0f, 1.0f,0.0f,1.0f},{0.0f,0.0f}},
                 {{ 1.0f, 1.0f,0.0f,1.0f},{1.0f,0.0f}},
                 {{ 1.0f,-1.0f,0.0f,1.0f},{1.0f,1.0f}},
                 {{-1.0f,-1.0f,0.0f,1.0f},{0.0f,1.0f}}
            };
            mVertexBuffer = std::make_unique<Framework::Graphics::VertexBuffer>(vertices);
        }

        {
            std::vector<UINT> indices{ 0,1,2,0,2,3 };
            mIndexBuffer = std::make_unique<Framework::Graphics::IndexBuffer>(indices);
        }

        //テクスチャ読み込み
        ComPtr<ID3D12Resource> textureUploadHeap;
        {
            constexpr UINT TEXTURE_PIXEL_SIZE = 4;
            static const std::string TEXTURE_NAME("bg.png");
            UINT WIDTH = 256;
            UINT HEIGHT = 256;
            //テクスチャの生成
            Framework::Utility::TextureLoader loader;
            std::vector<BYTE> data = loader.load((std::string)Framework::Define::Path::getInstance().texture + TEXTURE_NAME, &WIDTH, &HEIGHT);

            D3D12_RESOURCE_DESC desc{};
            desc.MipLevels = 1;
            desc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.Width = WIDTH;
            desc.Height = HEIGHT;
            desc.Flags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE;
            desc.DepthOrArraySize = 1;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.Dimension = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D;

            throwIfFailed(mDevice->CreateCommittedResource(
                &PROPERTY(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT),
                D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
                &desc,
                D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST,
                nullptr,
                IID_PPV_ARGS(&mTexture)));

            const UINT64 uploadBufferSize = getRequiredIntermediateSize(mTexture.Get(), 0, 1);

            //GPUアップロードバッファ作成
            throwIfFailed(mDevice->CreateCommittedResource(
                &PROPERTY(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD),
                D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
                &RESOURCE(uploadBufferSize),
                D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&textureUploadHeap)));

            std::vector<UINT8> texture = data;

            D3D12_SUBRESOURCE_DATA sub{};
            sub.pData = &texture[0];
            sub.RowPitch = WIDTH * TEXTURE_PIXEL_SIZE;
            sub.SlicePitch = sub.RowPitch * HEIGHT;
            updateSubresource(Framework::Graphics::DX12Manager::getInstance().getCommandList(), mTexture.Get(), textureUploadHeap.Get(), 0, 0, 1, &sub);

            Framework::Graphics::DX12Manager::getInstance().getCommandList()->ResourceBarrier(1, &BARRIER(mTexture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));


            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
            srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            srvDesc.Format = desc.Format;
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = 1;
            mDevice->CreateShaderResourceView(mTexture.Get(), &srvDesc, mSRVHeap->GetCPUDescriptorHandleForHeapStart());
        }

        //コンスタントバッファ作成
        ComPtr<ID3D12Resource> constantBufferHeap;
        {
            throwIfFailed(mDevice->CreateCommittedResource(
                &PROPERTY(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD),
                D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
                &RESOURCE(256),
                D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&mConstantBuffer)));

            D3D12_RANGE range{ 0,0 };
            throwIfFailed(mConstantBuffer->Map(0, &range, reinterpret_cast<void**>(&mCBVDataBegin)));
            memcpy(mCBVDataBegin, &mColorBuffer, sizeof(mColorBuffer));

            throwIfFailed(mDevice->CreateCommittedResource(
                &PROPERTY(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD),
                D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
                &RESOURCE(256),
                D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&mMatrixConstantBuffer)));

            throwIfFailed(mMatrixConstantBuffer->Map(0, &range, reinterpret_cast<void**>(&mMatrixCBVDataBegin)));
            memcpy(mMatrixCBVDataBegin, &mMVP, sizeof(mMVP));
        }

        Framework::Graphics::DX12Manager::getInstance().executeCommand();
        Framework::Graphics::DX12Manager::getInstance().createFence();
        return true;
    }
protected:
    virtual void update() override {
        Game::update();
        mColorBuffer.color.r += 0.01f;
        if (mColorBuffer.color.r >= 1.0f) mColorBuffer.color.r -= 1.0f;
        mColorBuffer.color.g += 0.005f;
        if (mColorBuffer.color.g >= 1.0f) mColorBuffer.color.g -= 1.0f;

        memcpy(mCBVDataBegin, &mColorBuffer, sizeof(mColorBuffer));

        using Framework::Math::Vector3;
        using Framework::Math::Matrix4x4;
        mMVP.world = Matrix4x4::transposition(Matrix4x4::createScale(Vector3(mScale, mScale, mScale)));
        mMVP.view = Matrix4x4::transposition(Matrix4x4::createView({ Vector3(0,0,-10),Vector3(0,0,0),Vector3(0,1,0) }));
        float ratio = static_cast<float>(Framework::Define::Config::getInstance().screenWidth) / static_cast<float>(Framework::Define::Config::getInstance().screenHeight);
        mMVP.proj = Matrix4x4::transposition(Matrix4x4::createProjection({ 45.0f,ratio,0.1f,1000.0f }));
        memcpy(mMatrixCBVDataBegin, &mMVP, sizeof(mMVP));
        mScale += 0.01f;

        if (Framework::Device::GameDevice::getInstance().getInputManager()->getKeyboard().getKeyDown(Framework::Input::KeyCode::A)) {
            mMode = !mMode;
        }
    }
    virtual void draw() override {
        Framework::Graphics::DX12Manager::getInstance().drawBegin();
        ID3D12GraphicsCommandList* mCommandList = Framework::Graphics::DX12Manager::getInstance().getCommandList();

        ID3D12DescriptorHeap* heaps[] = { mSRVHeap.Get() };
        mCommandList->SetDescriptorHeaps(_countof(heaps), heaps);

        mCommandList->SetGraphicsRootDescriptorTable(0, mSRVHeap->GetGPUDescriptorHandleForHeapStart());
        mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        mCommandList->SetGraphicsRootConstantBufferView(1, mConstantBuffer->GetGPUVirtualAddress());
        mCommandList->SetGraphicsRootConstantBufferView(2, mMatrixConstantBuffer->GetGPUVirtualAddress());
        mVertexBuffer->addToCommandList(mCommandList);
        mIndexBuffer->addToCommandList(mCommandList);
        mIndexBuffer->drawCall(mCommandList);

        Framework::Graphics::DX12Manager::getInstance().drawEnd();

    }
    void finalize() override {
        Framework::Graphics::DX12Manager::getInstance().finalize();
        Game::finalize();
    }

private:

private:
    struct ShaderObject {
        std::vector<BYTE> code;
    };
    ComPtr<ID3D12DescriptorHeap> mSRVHeap; //!< テクスチャSRV用
    ComPtr<ID3D12PipelineState> mPipelineState2; //!< パイプラインステート
    std::unique_ptr<Framework::Graphics::VertexBuffer> mVertexBuffer; //!< 頂点バッファ
    std::unique_ptr<Framework::Graphics::IndexBuffer> mIndexBuffer; //!< インデックスバッファ
    ComPtr<ID3D12Resource> mTexture; //!< テクスチャ
    ComPtr<ID3D12DescriptorHeap> mCBVHeap; //!< コンスタントバッファヒープ
    ComPtr<ID3D12Resource> mConstantBuffer; //!< コンスタントバッファ
    UINT* mCBVDataBegin;
    ComPtr<ID3D12DescriptorHeap> mMatrixCBVHeap; //!< コンスタントバッファヒープ
    ComPtr<ID3D12Resource> mMatrixConstantBuffer; //!< コンスタントバッファ
    UINT* mMatrixCBVDataBegin;
    ColorBuffer mColorBuffer;
    MVP mMVP;
    float mScale;
    bool mMode;
    UINT mNumIndices;
};

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPTSTR, _In_ int) {
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    MyGame game("Game", hInstance);
    return game.run();
}

