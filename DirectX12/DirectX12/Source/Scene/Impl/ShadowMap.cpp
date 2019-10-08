#include "ShadowMap.h"
#include <vector>
#include "Framework/Utility/IO/FBXLoader.h"
#include "Framework/Utility/IO/ShaderReader.h"
#include "Framework/Define/Path.h"
#include "Framework/Graphics/DX12/Resource/VertexBuffer.h"
#include "Framework/Graphics/DX12/Resource/IndexBuffer.h"
#include "Framework/Graphics/DX12/Resource/Texture.h"
#include "Framework/Graphics/DX12/Render/Pipeline.h"
#include "Framework/Graphics/DX12/RenderingManager.h"
#include "Framework/Graphics/DX12/Desc/BlendState.h"
#include "Framework/Graphics/DX12/Desc/Rasterizer.h"
#include "Framework/Define/Config.h"

using namespace Framework;

ShadowMap::ShadowMap() {
    Framework::Utility::FBXLoader loader((std::string)Framework::Define::Path::getInstance().fbx + "b4257199-b1a5-4e0b-991f-d73a8b8fa2b9.fbx");
    std::vector<Framework::Math::Vector4> pos = loader.getPosition();
    std::vector<UINT> indices(pos.size());
    for (int i = 0; i < indices.size() / 3; i++) {
        indices[i * 3 + 0] = i * 3 + 2;
        indices[i * 3 + 1] = i * 3 + 1;
        indices[i * 3 + 2] = i * 3 + 0;
    }

    mVertexBuffer = std::make_unique<Framework::Graphics::VertexBuffer>(pos);
    mIndexBuffer = std::make_unique<Framework::Graphics::IndexBuffer>(indices, Framework::Graphics::PrimitiveTolopolyType::TriangleList);

    mPipeline = std::make_unique<Graphics::Pipeline>(Graphics::RenderingManager::getInstance().getDX12Manager()->getMainRootSignature());
    Framework::Utility::ShaderReader vsReader((std::string)Framework::Define::Path::getInstance().shader + "3D/Only_Position_VS.cso");
    std::vector<BYTE> vs = vsReader.get();
    mPipeline->setVertexShader({ vs.data(),vs.size() });
    Framework::Utility::ShaderReader psReader((std::string)Framework::Define::Path::getInstance().shader + "3D/Only_Position_PS.cso");
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

    mMVP.view = Math::Matrix4x4::transposition(Math::Matrix4x4::createView({ Math::Vector3(0,5,-5),Math::Vector3(0,0,0),Math::Vector3(0,1,0) }));
    float ratio = static_cast<float>(Define::Config::getInstance().screenHeight) / static_cast<float>(Define::Config::getInstance().screenWidth);
    mMVP.proj = Math::Matrix4x4::transposition(Math::Matrix4x4::createProjection({ 45.0f,ratio,0.1f,100.0f }));
}

ShadowMap::~ShadowMap() { }

void ShadowMap::load(Framework::Scene::Collecter& collecter) { }

void ShadowMap::update() {
    mRotate += 1.0f;
    static const Math::Vector3 scale(5.0f, 5.0f, 5.0f);
    Math::Matrix4x4 mvp = Math::Matrix4x4::createScale(scale) * Math::Matrix4x4::createRotationY(mRotate);
    mMVP.world = Math::Matrix4x4::transposition(mvp);
}

bool ShadowMap::isEndScene() const {
    return false;
}

void ShadowMap::draw() {
    ID3D12GraphicsCommandList* mCommandList = Framework::Graphics::RenderingManager::getInstance().getDX12Manager()->getCommandList();
    Graphics::ConstantBufferManager* cbManager = Graphics::RenderingManager::getInstance().getConstantBufferManager();


    constexpr float RADIUS = 5.0f;

    mPipeline->addToCommandList(mCommandList);

    cbManager->beingCBufferUpdate();
    cbManager->updateCBuffer(mMVP);

    cbManager->endCBufferUpdate(mCommandList);

    mVertexBuffer->addToCommandList(mCommandList);
    mIndexBuffer->addToCommandList(mCommandList);
    mIndexBuffer->drawCall(mCommandList);
}

void ShadowMap::end() { }

Framework::Define::SceneType ShadowMap::next() {
    return Framework::Define::SceneType();
}
