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
#include "Framework/Graphics/DX12/Resource/VertexBuffer.h"
#include "Framework/Graphics/DX12/Resource/IndexBuffer.h"
#include "Framework/Graphics/DX12/Resource/ConstantBuffer.h"
#include "Framework/Graphics/DX12/Resource/Texture.h"
#include "Framework/Graphics/DX12/Helper.h"
#include "Framework/Graphics/DX12/Render/Pipeline.h"
#include "Framework/Graphics/DX12/Render/RootSignature.h"
#include "Framework/Graphics/DX12/Desc/Sampler.h"
#include "Framework/Utility/IO/ShaderReader.h"
#include "Framework/Define/Render.h"
#include "Framework/ImGUI/ImGUI.h"

namespace {
using namespace Framework::Graphics;
struct Vertex {
    Framework::Math::Vector4 pos;
    Framework::Math::Vector2 uv;
    Framework::Graphics::Color4 color;
};

struct ColorBuffer {
    Framework::Graphics::Color4 color;
};

struct MVP {
    Framework::Math::Matrix4x4 world;
    Framework::Math::Matrix4x4 view;
    Framework::Math::Matrix4x4 proj;
};

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

        mColorBuffer.color = Framework::Graphics::Color4(0.0f, 1.0f, 0.0f, 1.0f);

        UINT width = Framework::Define::Config::getInstance().screenWidth;
        UINT height = Framework::Define::Config::getInstance().screenHeight;
        Framework::Graphics::DX12Manager::getInstance().initialize(window->getHWND(), width, height);
        Framework::Graphics::DX12Manager::getInstance().executeCommand();
        Framework::Graphics::DX12Manager::getInstance().waitForPreviousFrame();
        ID3D12Device* mDevice = Framework::Graphics::DX12Manager::getInstance().getDevice();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark();
        ImGui_ImplWin32_Init(window->getHWND());

        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.NumDescriptors = 1;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        if (mDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&mImGUIDescriptorSrvHeap)) != S_OK)
            return false;

        ImGui_ImplDX12_Init(mDevice, 2, DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM,
            mImGUIDescriptorSrvHeap->GetCPUDescriptorHandleForHeapStart(),
            mImGUIDescriptorSrvHeap->GetGPUDescriptorHandleForHeapStart());

        //std::vector<Vertex> vertices
        //{
        //    {{-0.5f,   0.5f,  -0.5f,  1.0f} ,Framework::Graphics::Color4(1.0f,0.0f,0.0f,1.0f)},
        //    {{0.5f,    0.5f,  -0.5f,  1.0f} ,Framework::Graphics::Color4(0.0f,1.0f,0.0f,1.0f)},
        //    {{0.5f,    -0.5f,  -0.5f,  1.0f},Framework::Graphics::Color4(0.0f,0.0f,1.0f,1.0f) },
        //    {{-0.5f,   -0.5f,  -0.5f,  1.0f} ,Framework::Graphics::Color4(0.0f,1.0f,1.0f,1.0f)},
        //    {{-0.5f,    0.5f,   0.5f,  1.0f} ,Framework::Graphics::Color4(1.0f,1.0f,0.0f,1.0f)},
        //    {{0.5f,     0.5f,   0.5f,  1.0f} ,Framework::Graphics::Color4(1.0f,0.0f,1.0f,1.0f)},
        //    {{0.5f,    -0.5f,   0.5f,  1.0f},Framework::Graphics::Color4(1.0f,1.0f,1.0f,1.0f) },
        //    {{-0.5f,   -0.5f,   0.5f,  1.0f},Framework::Graphics::Color4(0.0f,0.0f,0.0f,1.0f) },
        //};
        //std::vector<UINT> indices
        //{
        //    0,1,2,
        //    0,2,3,
        //    4,0,3,
        //    4,3,7,
        //    5,4,7,
        //    5,7,6,
        //    1,5,6,
        //    1,6,2,
        //    3,2,6,
        //    3,6,7,
        //    4,5,1,
        //    4,1,0,
        //};
        std::vector<Vertex> vertices{
            {{-0.5f,0.5f,0.0f,1.0f},{0.0f,0.0f},Framework::Graphics::Color4::WHITE },
            {{0.5f,0.5f,0.0f,1.0f},{1.0f,0.0f},Framework::Graphics::Color4::WHITE },
            {{0.5f,-0.5f,0.0f,1.0f},{1.0f,1.0f},Framework::Graphics::Color4::WHITE },
            {{-0.5f,-0.5f,0.0f,1.0f},{0.0f,1.0f},Framework::Graphics::Color4::WHITE },
        };

        std::vector<UINT> indices{ 0,1,2,0,2,3 };
        mVertexBuffer = std::make_unique<Framework::Graphics::VertexBuffer>(vertices);
        mIndexBuffer = std::make_unique<Framework::Graphics::IndexBuffer>(indices, Framework::Graphics::PrimitiveTolopolyType::TriangleList);

        mTexture = std::make_unique<Framework::Graphics::Texture>((std::string)Framework::Define::Path::getInstance().texture + "bg.png");
        mTexture2 = std::make_unique<Framework::Graphics::Texture>((std::string)Framework::Define::Path::getInstance().texture + "bg2.png");

        //mColorConstantBuffer = std::make_unique<Framework::Graphics::ConstantBuffer>(mColorBuffer);
        //mMVPConstantBuffer = std::make_unique<Framework::Graphics::ConstantBuffer>(mMVP);

        //mRootSignature = std::make_shared<Framework::Graphics::RootSignature>();
        //mRootSignature->addConstantBufferParameter(Framework::Graphics::VisibilityType::All, 0);
        //mRootSignature->addConstantBufferParameter(Framework::Graphics::VisibilityType::All, 1);
        //mRootSignature->addConstantBufferParameter(Framework::Graphics::VisibilityType::All, 2);
        //mRootSignature->addTextureParameter(Framework::Graphics::VisibilityType::All, 0);
        //mRootSignature->addStaticSamplerParameter(Framework::Graphics::Sampler::createStaticSampler(Framework::Graphics::FilterMode::Linear, Framework::Graphics::AddressMode::Wrap, Framework::Graphics::VisibilityType::Pixel, 0));
        //mRootSignature->createDX12RootSignature();

        //mPipeline = std::make_unique<Framework::Graphics::Pipeline>(mRootSignature);
        //Framework::Utility::ShaderReader vsReader((std::string)Framework::Define::Path::getInstance().shader + "2D/Texture2D_VS.cso");
        //mPipeline->setVertexShader({ vsReader.get().data(),vsReader.get().size() });
        //mPipeline->setInputLayout({ vsReader.getShaderReflection().data(),(UINT)vsReader.getShaderReflection().size() });
        //Framework::Utility::ShaderReader psReader((std::string)Framework::Define::Path::getInstance().shader + "2D/Texture2D_PS.cso");
        //mPipeline->setPixelShader({ psReader.get().data(),psReader.get().size() });
        //mPipeline->setPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);


        //CBV
        D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc{};
        cbvHeapDesc.NumDescriptors = Framework::Define::Render::MAX_CONSTANT_BUFFER_USE_NUM_PER_ONE_FRAME;
        cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        throwIfFailed(DXInterfaceAccessor::getDevice()->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&mCBVHeap)));
        UINT size = Framework::Graphics::sizeAlignment(sizeof(MVP)) * Framework::Define::Render::MAX_CONSTANT_BUFFER_USE_NUM_PER_ONE_FRAME;
        throwIfFailed(DXInterfaceAccessor::getDevice()->CreateCommittedResource(
            &createProperty(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
            &createResource(size),
            D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&mConstantBuffer)));

        struct { char buf[256]; } *mCBVDataBegin;
        D3D12_RANGE range{ 0,0 };
        throwIfFailed(mConstantBuffer->Map(0, &range, reinterpret_cast<void**>(&mCBVDataBegin)));
        memcpy(mCBVDataBegin, &mMVP, sizeof(MVP));
        memcpy(mCBVDataBegin + 1, &mColorBuffer, sizeof(Color4));
        mConstantBuffer->Unmap(0, nullptr);

        D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
        srvHeapDesc.NumDescriptors = Framework::Define::Render::MAX_TEXTURE_USE_NUM_PER_ONE_FRAME;
        srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        throwIfFailed(DXInterfaceAccessor::getDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&mSRVHeap)));

        mAlphaTheta = 0.0f;
        return true;
    }
protected:
    virtual void update() override {
        using Framework::Math::Vector3;
        using Framework::Math::Matrix4x4;
        Game::update();

        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("HELLO WORLD!");
        ImGui::Text("TEXT");
        static float f = 0.0f;
        ImGui::SliderFloat("R", &f, 0.0f, 1.0f);
        mColorBuffer.color.r = f;
        ImGui::End();

        mMVP.world = Matrix4x4::transposition(Matrix4x4::createTranslate(Vector3(0, 0, 0)));
        mMVP.view = Matrix4x4::transposition(Matrix4x4::createView({ Vector3(0,0,-10),Vector3(0,0,0),Vector3(0,1,0) }));
        float ratio = static_cast<float>(Framework::Define::Config::getInstance().screenWidth) / static_cast<float>(Framework::Define::Config::getInstance().screenHeight);
        mMVP.proj = Matrix4x4::transposition(Matrix4x4::createProjection({ 45.0f,ratio,0.1f,1000.0f }));

        if (Framework::Device::GameDevice::getInstance().getInputManager()->getKeyboard().getKeyDown(Framework::Input::KeyCode::A)) {
            mMode = !mMode;
        }
    }
    virtual void draw() override {
        using Framework::Math::Vector3;
        using Framework::Math::Matrix4x4;

        Framework::Graphics::DX12Manager::getInstance().drawBegin();
        ID3D12GraphicsCommandList* mCommandList = Framework::Graphics::DX12Manager::getInstance().getCommandList();
        if (mMode) {
            DXInterfaceAccessor::getDevice()->CreateShaderResourceView(mTexture->mTexture.Get(), nullptr, mSRVHeap->GetCPUDescriptorHandleForHeapStart());
        }
        else {
            DXInterfaceAccessor::getDevice()->CreateShaderResourceView(mTexture2->mTexture.Get(), nullptr, mSRVHeap->GetCPUDescriptorHandleForHeapStart());
        }

        struct { char buf[256]; } *mCBVDataBegin;
        D3D12_RANGE range{ 0,0 };
        throwIfFailed(mConstantBuffer->Map(0, &range, reinterpret_cast<void**>(&mCBVDataBegin)));
        memcpy(mCBVDataBegin, &mMVP, sizeof(MVP));
        memcpy(mCBVDataBegin + 1, &mColorBuffer, sizeof(Color4));
        mMVP.world = Matrix4x4::transposition(Matrix4x4::createTranslate(Vector3(1.0f, 0, 0)));
        memcpy(mCBVDataBegin + 2, &mMVP, sizeof(MVP));
        memcpy(mCBVDataBegin + 3, &mColorBuffer, sizeof(Color4));
        {
            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
            cbvDesc.BufferLocation = mConstantBuffer->GetGPUVirtualAddress();
            cbvDesc.SizeInBytes = sizeAlignment(sizeof(MVP));

            D3D12_CPU_DESCRIPTOR_HANDLE ptr = mCBVHeap->GetCPUDescriptorHandleForHeapStart();
            ptr.ptr += 0 * (DXInterfaceAccessor::getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
            DXInterfaceAccessor::getDevice()->CreateConstantBufferView(&cbvDesc, ptr);
        }
        {
            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
            cbvDesc.BufferLocation = mConstantBuffer->GetGPUVirtualAddress() + 0x100 * 1;
            cbvDesc.SizeInBytes = sizeAlignment(sizeof(Color4));

            D3D12_CPU_DESCRIPTOR_HANDLE ptr = mCBVHeap->GetCPUDescriptorHandleForHeapStart();
            ptr.ptr += 1 * (DXInterfaceAccessor::getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
            DXInterfaceAccessor::getDevice()->CreateConstantBufferView(&cbvDesc, ptr);
        }
        ID3D12DescriptorHeap* heaps[] = { mCBVHeap.Get(), };
        mCommandList->SetDescriptorHeaps(_countof(heaps), heaps);

        mCommandList->SetGraphicsRootDescriptorTable(0, mCBVHeap->GetGPUDescriptorHandleForHeapStart());

        ID3D12DescriptorHeap* heaps2[] = { mSRVHeap.Get(), };
        mCommandList->SetDescriptorHeaps(_countof(heaps2), heaps2);

        mCommandList->SetGraphicsRootDescriptorTable(1, mSRVHeap->GetGPUDescriptorHandleForHeapStart());

        mVertexBuffer->addToCommandList(mCommandList);
        mIndexBuffer->addToCommandList(mCommandList);
        mIndexBuffer->drawCall(mCommandList);



        {
            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
            cbvDesc.BufferLocation = mConstantBuffer->GetGPUVirtualAddress() + 0x100 * 2;
            cbvDesc.SizeInBytes = sizeAlignment(sizeof(MVP));

            D3D12_CPU_DESCRIPTOR_HANDLE ptr = mCBVHeap->GetCPUDescriptorHandleForHeapStart();
            ptr.ptr += 2 * (DXInterfaceAccessor::getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
            DXInterfaceAccessor::getDevice()->CreateConstantBufferView(&cbvDesc, ptr);
        }

        {
            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
            cbvDesc.BufferLocation = mConstantBuffer->GetGPUVirtualAddress() + 0x100 * 3;
            cbvDesc.SizeInBytes = sizeAlignment(sizeof(Color4));

            D3D12_CPU_DESCRIPTOR_HANDLE ptr = mCBVHeap->GetCPUDescriptorHandleForHeapStart();
            ptr.ptr += 3 * (DXInterfaceAccessor::getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
            DXInterfaceAccessor::getDevice()->CreateConstantBufferView(&cbvDesc, ptr);
        }



        ID3D12DescriptorHeap* heap3[] = { mCBVHeap.Get() };
        mCommandList->SetDescriptorHeaps(_countof(heap3), heap3);
        D3D12_GPU_DESCRIPTOR_HANDLE addr = mCBVHeap->GetGPUDescriptorHandleForHeapStart();
        addr.ptr += 2 * (DXInterfaceAccessor::getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
        mCommandList->SetGraphicsRootDescriptorTable(0, addr);

        mVertexBuffer->addToCommandList(mCommandList);
        mIndexBuffer->addToCommandList(mCommandList);
        mIndexBuffer->drawCall(mCommandList);

        mConstantBuffer->Unmap(0, nullptr);

        mCommandList->SetDescriptorHeaps(1, mImGUIDescriptorSrvHeap.GetAddressOf());
        ImGui::Render();
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), mCommandList);

        Framework::Graphics::DX12Manager::getInstance().drawEnd();

    }
    void finalize() override {
        ImGui_ImplDX12_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        Framework::Graphics::DX12Manager::getInstance().finalize();
        Game::finalize();
    }

private:

private:
    struct ShaderObject {
        std::vector<BYTE> code;
    };
    std::unique_ptr<Framework::Graphics::Texture> mTexture; //!< テクスチャ
    std::unique_ptr<Framework::Graphics::Texture> mTexture2; //!< テクスチャ

    //std::unique_ptr<Framework::Graphics::Pipeline> mPipeline;
    //std::shared_ptr<Framework::Graphics::RootSignature> mRootSignature;

    std::unique_ptr<Framework::Graphics::VertexBuffer> mVertexBuffer; //!< 頂点バッファ
    std::unique_ptr<Framework::Graphics::IndexBuffer> mIndexBuffer; //!< インデックスバッファ
    //std::unique_ptr<Framework::Graphics::ConstantBuffer> mColorConstantBuffer; //!< コンスタントバッファ
    //std::unique_ptr<Framework::Graphics::ConstantBuffer> mMVPConstantBuffer; //!< コンスタントバッファ
    ColorBuffer mColorBuffer;
    ComPtr<ID3D12Resource> mConstantBuffer;
    ComPtr<ID3D12DescriptorHeap> mCBVHeap;
    ComPtr<ID3D12DescriptorHeap> mSRVHeap;

    ComPtr<ID3D12DescriptorHeap> mImGUIDescriptorSrvHeap;

    float mAlphaTheta;
    MVP mMVP;
    float mRotate;
    bool mMode;
    UINT mNumIndices;
};

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPTSTR, _In_ int) {
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    MyGame game("Game", hInstance);
    return game.run();
}

