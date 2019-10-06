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
#include "Framework/Window/Procedure/ImguiProc.h"
#include "Framework/Utility/IO/TextureLoader.h"
#include "Framework/Math/Matrix4x4.h"
#include "Framework/Graphics/DX12/Desc/BlendState.h"
#include "Framework/Graphics/DX12/Desc/Rasterizer.h"
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
#include "Framework/Utility/Time.h"
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
        window->addProcedureEvent(new ImguiProc());
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

        ImGui_ImplDX12_Init(mDevice, 2, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
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


        mPipeline = std::make_unique<Pipeline>(Framework::Graphics::DX12Manager::getInstance().getMainRootSignature());
        Framework::Utility::ShaderReader vsReader((std::string)Framework::Define::Path::getInstance().shader + "VertexShader.cso");
        std::vector<BYTE> vs = vsReader.get();
        mPipeline->setVertexShader({ vs.data(),vs.size() });
        Framework::Utility::ShaderReader psReader((std::string)Framework::Define::Path::getInstance().shader + "PixelShader2.cso");
        std::vector<BYTE> ps = psReader.get();
        mPipeline->setPixelShader({ ps.data(),ps.size() });
        std::vector<D3D12_INPUT_ELEMENT_DESC> elem = vsReader.getShaderReflection();
        mPipeline->setInputLayout({ elem.data(),(UINT)elem.size() });
        D3D12_BLEND_DESC bd{};
        bd.AlphaToCoverageEnable = FALSE;
        bd.IndependentBlendEnable = FALSE;
        for (int i = 0; i < 8; i++) {
            bd.RenderTarget[i] = Framework::Graphics::BlendState::alignmentBlendDesc();
        }

        mPipeline->setRenderTarget({ DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM });
        mPipeline->setSampleDesc({ 1,0 });
        mPipeline->setSampleMask(UINT_MAX);
        mPipeline->setPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
        mPipeline->setBlendState(bd);
        mPipeline->setRasterizerState(Framework::Graphics::Rasterizer(Framework::Graphics::FillMode::Solid, Framework::Graphics::CullMode::Back));
        mPipeline->createPipelineState();

        //CBV
        D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc{ };
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
        mObjectNum = 2;
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

        ImGui::Begin("FPS");
        ImGui::Text("FPS %.3f", (float)Framework::Utility::Time::getInstance().currentFPS);
        ImGui::End();

        ImGui::Begin("PARAMETER");
        ImGui::SliderInt("NUM", &mObjectNum, 0, 10000);
        ImGui::SliderFloat4("COLOR", (float*)&mColorBuffer.color, 0.0f, 1.0f);
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

        ID3D12DescriptorHeap* heaps2[] = { mSRVHeap.Get(), };
        mCommandList->SetDescriptorHeaps(_countof(heaps2), heaps2);

        mCommandList->SetGraphicsRootDescriptorTable(1, mSRVHeap->GetGPUDescriptorHandleForHeapStart());

        struct { char buf[256]; } *mCBVDataBegin;
        D3D12_RANGE range{ 0,0 };
        throwIfFailed(mConstantBuffer->Map(0, &range, reinterpret_cast<void**>(&mCBVDataBegin)));

        for (int i = 0; i < mObjectNum; i++) {
            if (i == mObjectNum / 2) {
                mPipeline->addToCommandList(mCommandList);
            }
            int mvpOffset = i * 2;
            int colorOffset = i * 2 + 1;
            float theta = 360.0f * i / mObjectNum;
            float sin = Framework::Math::MathUtil::sin(theta);
            float cos = Framework::Math::MathUtil::cos(theta);
            mMVP.world = Matrix4x4::transposition(Matrix4x4::createTranslate(Vector3(cos, sin, 0)));
            memcpy(mCBVDataBegin + mvpOffset, &mMVP, sizeof(MVP));
            memcpy(mCBVDataBegin + colorOffset, &mColorBuffer, sizeof(Color4));
            {
                D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
                cbvDesc.BufferLocation = mConstantBuffer->GetGPUVirtualAddress() + 0x100 * mvpOffset;
                cbvDesc.SizeInBytes = sizeAlignment(sizeof(MVP));

                D3D12_CPU_DESCRIPTOR_HANDLE ptr = mCBVHeap->GetCPUDescriptorHandleForHeapStart();
                ptr.ptr += mvpOffset * (DXInterfaceAccessor::getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
                DXInterfaceAccessor::getDevice()->CreateConstantBufferView(&cbvDesc, ptr);
            }
            {
                D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
                cbvDesc.BufferLocation = mConstantBuffer->GetGPUVirtualAddress() + 0x100 * colorOffset;
                cbvDesc.SizeInBytes = sizeAlignment(sizeof(Color4));

                D3D12_CPU_DESCRIPTOR_HANDLE ptr = mCBVHeap->GetCPUDescriptorHandleForHeapStart();
                ptr.ptr += colorOffset * (DXInterfaceAccessor::getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
                DXInterfaceAccessor::getDevice()->CreateConstantBufferView(&cbvDesc, ptr);
            }

            ID3D12DescriptorHeap* heaps[] = { mCBVHeap.Get(), };
            mCommandList->SetDescriptorHeaps(_countof(heaps), heaps);
            D3D12_GPU_DESCRIPTOR_HANDLE addr = mCBVHeap->GetGPUDescriptorHandleForHeapStart();
            addr.ptr += mvpOffset * (DXInterfaceAccessor::getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
            mCommandList->SetGraphicsRootDescriptorTable(0, addr);

            {
                D3D12_CPU_DESCRIPTOR_HANDLE ptr = mSRVHeap->GetCPUDescriptorHandleForHeapStart();
                ptr.ptr += i * (DXInterfaceAccessor::getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
                //if (i % 2 == 0) {
                    DXInterfaceAccessor::getDevice()->CreateShaderResourceView(mTexture->mTexture.Get(), nullptr, ptr);
                //}
                //else {
                    //DXInterfaceAccessor::getDevice()->CreateShaderResourceView(mTexture2->mTexture.Get(), nullptr, ptr);
                //}
                ID3D12DescriptorHeap* heaps2[] = { mSRVHeap.Get(), };
                mCommandList->SetDescriptorHeaps(_countof(heaps2), heaps2);
            }
            {
                D3D12_GPU_DESCRIPTOR_HANDLE ptr = mSRVHeap->GetGPUDescriptorHandleForHeapStart();
                ptr.ptr += i * (DXInterfaceAccessor::getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
                mCommandList->SetGraphicsRootDescriptorTable(1, ptr);
            }


            mVertexBuffer->addToCommandList(mCommandList);
            mIndexBuffer->addToCommandList(mCommandList);
            mIndexBuffer->drawCall(mCommandList);
        }

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

    std::unique_ptr<Framework::Graphics::VertexBuffer> mVertexBuffer; //!< 頂点バッファ
    std::unique_ptr<Framework::Graphics::IndexBuffer> mIndexBuffer; //!< インデックスバッファ
    std::unique_ptr<Pipeline> mPipeline;
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
    int mObjectNum;
};

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPTSTR, _In_ int) {
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    MyGame game("Game", hInstance);
    return game.run();
}

