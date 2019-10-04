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

namespace {
using namespace Framework::Graphics;
struct Vertex {
    Framework::Math::Vector4 pos;
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

        mColorBuffer.color = Framework::Graphics::Color4(1.0f, 1.0f, 1.0f, 1.0f);

        UINT width = Framework::Define::Config::getInstance().screenWidth;
        UINT height = Framework::Define::Config::getInstance().screenHeight;
        Framework::Graphics::DX12Manager::getInstance().initialize(window->getHWND(), width, height);
        Framework::Graphics::DX12Manager::getInstance().executeCommand();
        Framework::Graphics::DX12Manager::getInstance().waitForPreviousFrame();
        ID3D12Device* mDevice = Framework::Graphics::DX12Manager::getInstance().getDevice();

        std::vector<Vertex> vertices
        {
            {{-0.5f,   0.5f,  -0.5f,  1.0f} ,Framework::Graphics::Color4(1.0f,0.0f,0.0f,1.0f)},
            {{0.5f,    0.5f,  -0.5f,  1.0f} ,Framework::Graphics::Color4(0.0f,1.0f,0.0f,1.0f)},
            {{0.5f,    -0.5f,  -0.5f,  1.0f},Framework::Graphics::Color4(0.0f,0.0f,1.0f,1.0f) },
            {{-0.5f,   -0.5f,  -0.5f,  1.0f} ,Framework::Graphics::Color4(0.0f,1.0f,1.0f,1.0f)},
            {{-0.5f,    0.5f,   0.5f,  1.0f} ,Framework::Graphics::Color4(1.0f,1.0f,0.0f,1.0f)},
            {{0.5f,     0.5f,   0.5f,  1.0f} ,Framework::Graphics::Color4(1.0f,0.0f,1.0f,1.0f)},
            {{0.5f,    -0.5f,   0.5f,  1.0f},Framework::Graphics::Color4(1.0f,1.0f,1.0f,1.0f) },
            {{-0.5f,   -0.5f,   0.5f,  1.0f},Framework::Graphics::Color4(0.0f,0.0f,0.0f,1.0f) },
        };
        std::vector<UINT> indices
        {
            0,1,2,
            0,2,3,
            4,0,3,
            4,3,7,
            5,4,7,
            5,7,6,
            1,5,6,
            1,6,2,
            3,2,6,
            3,6,7,
            4,5,1,
            4,1,0,
        };

        mVertexBuffer = std::make_unique<Framework::Graphics::VertexBuffer>(vertices);
        mIndexBuffer = std::make_unique<Framework::Graphics::IndexBuffer>(indices, Framework::Graphics::PrimitiveTolopolyType::TriangleList);

        mTexture = std::make_unique<Framework::Graphics::Texture>((std::string)Framework::Define::Path::getInstance().texture + "bg.png");
        mTexture2 = std::make_unique<Framework::Graphics::Texture>((std::string)Framework::Define::Path::getInstance().texture + "bg2.png");

        mColorConstantBuffer = std::make_unique<Framework::Graphics::ConstantBuffer>(mColorBuffer);
        mMVPConstantBuffer = std::make_unique<Framework::Graphics::ConstantBuffer>(mMVP);

        return true;
    }
protected:
    virtual void update() override {
        Game::update();
        mColorBuffer.color.r += 0.01f;
        if (mColorBuffer.color.r >= 1.0f) mColorBuffer.color.r -= 1.0f;
        mColorBuffer.color.g += 0.005f;
        if (mColorBuffer.color.g >= 1.0f) mColorBuffer.color.g -= 1.0f;

        //mColorConstantBuffer->updateBuffer(mColorBuffer);

        using Framework::Math::Vector3;
        using Framework::Math::Matrix4x4;
        mMVP.world = Matrix4x4::transposition(Matrix4x4::createRotationZ(mRotate) * Matrix4x4::createRotationY(mRotate * 2));
        mMVP.view = Matrix4x4::transposition(Matrix4x4::createView({ Vector3(0,0,-10),Vector3(0,0,0),Vector3(0,1,0) }));
        float ratio = static_cast<float>(Framework::Define::Config::getInstance().screenWidth) / static_cast<float>(Framework::Define::Config::getInstance().screenHeight);
        mMVP.proj = Matrix4x4::transposition(Matrix4x4::createProjection({ 45.0f,ratio,0.1f,1000.0f }));
        mMVPConstantBuffer->updateBuffer(mMVP);
        mRotate += 0.1f;

        if (Framework::Device::GameDevice::getInstance().getInputManager()->getKeyboard().getKeyDown(Framework::Input::KeyCode::A)) {
            mMode = !mMode;
        }
    }
    virtual void draw() override {
        Framework::Graphics::DX12Manager::getInstance().drawBegin();
        ID3D12GraphicsCommandList* mCommandList = Framework::Graphics::DX12Manager::getInstance().getCommandList();

        if (mMode) {
            ID3D12DescriptorHeap* heaps[] = { mTexture->getDescriptorHeap() };
            mCommandList->SetDescriptorHeaps(_countof(heaps), heaps);
            mTexture->addToCommandList(mCommandList, 0);
        }
        else {
            ID3D12DescriptorHeap* heaps[] = { mTexture2->getDescriptorHeap() };
            mCommandList->SetDescriptorHeaps(_countof(heaps), heaps);
            mTexture2->addToCommandList(mCommandList, 0);
        }
        mColorConstantBuffer->addToCommandList(mCommandList, 1);
        mMVPConstantBuffer->addToCommandList(mCommandList, 2);
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
    std::unique_ptr<Framework::Graphics::Texture> mTexture; //!< テクスチャ
    std::unique_ptr<Framework::Graphics::Texture> mTexture2; //!< テクスチャ
    ComPtr<ID3D12PipelineState> mPipelineState2; //!< パイプラインステート

    std::unique_ptr<Framework::Graphics::VertexBuffer> mVertexBuffer; //!< 頂点バッファ
    std::unique_ptr<Framework::Graphics::IndexBuffer> mIndexBuffer; //!< インデックスバッファ
    std::unique_ptr<Framework::Graphics::ConstantBuffer> mColorConstantBuffer; //!< コンスタントバッファ
    std::unique_ptr<Framework::Graphics::ConstantBuffer> mMVPConstantBuffer; //!< コンスタントバッファ
    ColorBuffer mColorBuffer;
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

