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
#include "Framework/Scene/Manager.h"
#include "Scene/Impl/ExecuteIndirect.h"
#include "Scene/Impl/RenderWhiteModel.h"
#include "Scene/Impl/RenderSamplingTexture.h"
#include "Scene/Impl/ShadowMap.h"

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

        UINT width = Framework::Define::Config::getInstance().screenWidth;
        UINT height = Framework::Define::Config::getInstance().screenHeight;
        Framework::Graphics::RenderingManager::getInstance().init(window->getHWND(), width, height);

        mSceneManager = std::make_unique<Framework::Scene::Manager>();
        mSceneManager->registerScene(Framework::Define::SceneType::ShadowMap, std::make_unique<ShadowMap>());
        //mSceneManager->registerScene(Framework::Define::SceneType::ExecuteIndirect, std::make_unique<ExecuteIndirect>(window->getHWND()));
        //mSceneManager->registerScene(Framework::Define::SceneType::RenderWhiteModel, std::make_unique<RenderWhiteModel>());
        //mSceneManager->registerScene(Framework::Define::SceneType::RenderSamplingTexture, std::make_unique<RenderSamplingTexture>());
        mSceneManager->loadScene(Framework::Define::SceneType::ShadowMap);

        ID3D12Device* device = DXInterfaceAccessor::getDevice();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark();
        ImGui_ImplWin32_Init(window->getHWND());

        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.NumDescriptors = 1;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        if (device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&mImGUIDescriptorSrvHeap)) != S_OK)
            return false;

        ImGui_ImplDX12_Init(device, 2, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
            mImGUIDescriptorSrvHeap->GetCPUDescriptorHandleForHeapStart(),
            mImGUIDescriptorSrvHeap->GetGPUDescriptorHandleForHeapStart());

        //Framework::Graphics::RenderingManager::getInstance().getDX12Manager()->executeCommand();
        return true;
    }
protected:
    virtual void update() override {
        Game::update();
        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        mSceneManager->update();

        ImGui::Begin("FPS");
        ImGui::Text("FPS %.3f", (float)Framework::Utility::Time::getInstance().currentFPS);
        ImGui::End();

    }
    virtual void draw() override {
        Framework::Graphics::RenderingManager::getInstance().begin();
        ID3D12GraphicsCommandList* mCommandList = Framework::Graphics::RenderingManager::getInstance().getDX12Manager()->getCommandList();

        mSceneManager->draw();


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
    std::unique_ptr<Framework::Scene::Manager> mSceneManager;
    ComPtr<ID3D12DescriptorHeap> mImGUIDescriptorSrvHeap;
};

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPTSTR, _In_ int) {
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    MyGame game("Game", hInstance);
    return game.run();
}
