#pragma once
#include "Framework/Scene/SceneBase.h"
#include "Framework/Graphics/DX12/Material/CBStruct.h"

namespace Framework {
namespace Graphics {
class VertexBuffer;
class IndexBuffer;
class Pipeline;
class Texture;
} //Graphics 
} //Framework 

/**
* @class RenderSamplingTexture
* @brief discription
*/
class RenderSamplingTexture : public Framework::Scene::SceneBase {
public:
    /**
    * @brief コンストラクタ
    */
    RenderSamplingTexture();
    /**
    * @brief デストラクタ
    */
    ~RenderSamplingTexture();
    /**
    * @brief シーンデータの読み込み
    */
    virtual void load(Framework::Scene::Collecter& collecter) override;
    /**
    * @brief 更新
    */
    virtual void update() override;
    /**
    * @brief 終了しているか
    */
    virtual bool isEndScene() const override;
    /**
    * @brief 描画
    */
    virtual void draw() override;
    /**
    * @brief 終了処理
    */
    virtual void end() override;
    /**
    * @brief 次のシーン
    */
    virtual Framework::Define::SceneType next() override;
private:
    std::unique_ptr<Framework::Graphics::VertexBuffer> mVertexBuffer; //!< 頂点バッファ
    std::unique_ptr<Framework::Graphics::IndexBuffer> mIndexBuffer; //!< インデックスバッファ
    std::unique_ptr<Framework::Graphics::Pipeline> mPipeline;
    std::shared_ptr<Framework::Graphics::Texture> mTexture;
    Framework::Graphics::MVPCBuffer mMVP;
    float mRotate;
};
