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
#include "Framework/Graphics/DX12/RenderingManager.h"
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
#include "Framework/Graphics/DX12/Material/CBStruct.h"
#include "Framework/Graphics/DX12/Resource/ConstantBuffer.h"
#include "Framework/Graphics/DX12/Resource/ShaderResourceView.h"
#include "Framework/Utility/IO/FBXLoader.h"

namespace {
using namespace Framework::Graphics;
struct Vertex {
    Framework::Math::Vector4 pos;
    Framework::Math::Vector2 uv;
    Framework::Graphics::Color4 color;
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
        Framework::Graphics::RenderingManager::getInstance().init(window->getHWND(), width, height);
        ID3D12Device* mDevice = DXInterfaceAccessor::getDevice();

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

        //std::vector<Vertex> vertices{
        //    {{-0.5f,0.5f,0.0f,1.0f},{0.0f,0.0f},Framework::Graphics::Color4::WHITE },
        //    {{0.5f,0.5f,0.0f,1.0f},{1.0f,0.0f},Framework::Graphics::Color4::WHITE },
        //    {{0.5f,-0.5f,0.0f,1.0f},{1.0f,1.0f},Framework::Graphics::Color4::WHITE },
        //    {{-0.5f,-0.5f,0.0f,1.0f},{0.0f,1.0f},Framework::Graphics::Color4::WHITE },
        //};

        //std::vector<UINT> indices{ 0,1,2,0,2,3 };
        //Framework::Utility::FBXLoader loader((std::string)Framework::Define::Path::getInstance().fbx + "531965ba-dac1-46dd-bf89-82824f288cf0.fbx");
        Framework::Utility::FBXLoader loader((std::string)Framework::Define::Path::getInstance().fbx + "b4257199-b1a5-4e0b-991f-d73a8b8fa2b9.fbx");
        std::vector<Framework::Math::Vector4> pos = loader.getPosition();
        //std::vector<Framework::Math::Vector2> uv = loader.getUV();
        std::vector<Vertex> vertices(pos.size());
        for (int i = 0; i < pos.size(); i++) {
            vertices[i].pos = pos[i];
            //vertices[i].uv = uv[i];
        }
        std::vector<UINT> indices(pos.size());
        for (int i = 0; i < indices.size() / 3; i++) {
            indices[i * 3 + 0] = i * 3 + 2;
            indices[i * 3 + 1] = i * 3 + 1;
            indices[i * 3 + 2] = i * 3 + 0;
        }

        mVertexBuffer = std::make_unique<Framework::Graphics::VertexBuffer>(vertices);
        mIndexBuffer = std::make_unique<Framework::Graphics::IndexBuffer>(indices, Framework::Graphics::PrimitiveTolopolyType::TriangleList);

        mTexture = std::make_unique<Framework::Graphics::Texture>((std::string)Framework::Define::Path::getInstance().fbx + "item/textures/uv.png");
        //mTexture2 = std::make_unique<Framework::Graphics::Texture>((std::string)Framework::Define::Path::getInstance().texture + "bg2.png");

        mPipeline = std::make_unique<Pipeline>(Framework::Graphics::RenderingManager::getInstance().getDX12Manager()->getMainRootSignature());
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
            bd.RenderTarget[i] = Framework::Graphics::BlendState::addBlendDesc();
        }

        mPipeline->setRenderTarget({ DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM });
        mPipeline->setSampleDesc({ 1,0 });
        mPipeline->setSampleMask(UINT_MAX);
        mPipeline->setPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
        mPipeline->setBlendState(bd);
        mPipeline->setRasterizerState(Framework::Graphics::Rasterizer(Framework::Graphics::FillMode::Solid, Framework::Graphics::CullMode::Back));
        mPipeline->createPipelineState();

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
        ImGui::SliderInt("NUM", &mObjectNum, 0, 20000);
        ImGui::SliderFloat4("COLOR", (float*)&mColorBuffer.color, 0.0f, 1.0f);
        ImGui::End();

        mMVP.world = Matrix4x4::transposition(Matrix4x4::createTranslate(Vector3(0, 0, 0)));
        mMVP.view = Matrix4x4::transposition(Matrix4x4::createView({ Vector3(0,5,-5),Vector3(0,0,0),Vector3(0,1,0) }));
        float ratio = static_cast<float>(Framework::Define::Config::getInstance().screenHeight) / static_cast<float>(Framework::Define::Config::getInstance().screenWidth);
        mMVP.proj = Matrix4x4::transposition(Matrix4x4::createProjection({ 45.0f,ratio,0.1f,100.0f }));
        mAlphaTheta += 10.0f *Framework::Utility::Time::getInstance().deltaTime;
        if (Framework::Device::GameDevice::getInstance().getInputManager()->getKeyboard().getKeyDown(Framework::Input::KeyCode::A)) {
            mMode = !mMode;
        }
    }
    virtual void draw() override {
        using Framework::Math::Vector3;
        using Framework::Math::Matrix4x4;

        Framework::Graphics::RenderingManager::getInstance().begin();
        ConstantBufferManager* cbManager = RenderingManager::getInstance().getConstantBufferManager();
        SRVManager* srvManager = RenderingManager::getInstance().getSRVManager();

        ID3D12GraphicsCommandList* mCommandList = Framework::Graphics::RenderingManager::getInstance().getDX12Manager()->getCommandList();

        constexpr float RADIUS = 5.0f;

        for (int i = 0; i < 1; i++) {
            mPipeline->addToCommandList(mCommandList);
            mMVP.world = Matrix4x4::transposition(
                Matrix4x4::createScale(Vector3(5.0f, 5.0f, 5.0f)) *
                Matrix4x4::createRotationY(mAlphaTheta)
            );

            cbManager->beingCBufferUpdate();
            cbManager->updateCBuffer(mMVP);

            cbManager->endCBufferUpdate(mCommandList);

            srvManager->beginUpdate();
            srvManager->setResource(mTexture);
            //srvManager->setResource(mTexture2);
            srvManager->endUpdate(mCommandList);

            mVertexBuffer->addToCommandList(mCommandList);
            mIndexBuffer->addToCommandList(mCommandList);
            mIndexBuffer->drawCall(mCommandList);
        }


        mCommandList->SetDescriptorHeaps(1, mImGUIDescriptorSrvHeap.GetAddressOf());
        ImGui::Render();
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), mCommandList);
        Framework::Graphics::RenderingManager::getInstance().end();

    }
    void finalize() override {
        ImGui_ImplDX12_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        Game::finalize();
    }

private:

private:
    struct ShaderObject {
        std::vector<BYTE> code;
    };
    std::shared_ptr<Framework::Graphics::Texture> mTexture; //!< テクスチャ
    std::shared_ptr<Framework::Graphics::Texture> mTexture2; //!< テクスチャ

    std::unique_ptr<Framework::Graphics::VertexBuffer> mVertexBuffer; //!< 頂点バッファ
    std::unique_ptr<Framework::Graphics::IndexBuffer> mIndexBuffer; //!< インデックスバッファ
    std::unique_ptr<Pipeline> mPipeline;
    Framework::Graphics::ColorCBuffer mColorBuffer;
    ComPtr<ID3D12DescriptorHeap> mImGUIDescriptorSrvHeap;
    float mAlphaTheta;
    Framework::Graphics::MVPCBuffer mMVP;
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

