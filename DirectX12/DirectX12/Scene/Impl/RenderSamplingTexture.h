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
    * @brief �R���X�g���N�^
    */
    RenderSamplingTexture();
    /**
    * @brief �f�X�g���N�^
    */
    ~RenderSamplingTexture();
    /**
    * @brief �V�[���f�[�^�̓ǂݍ���
    */
    virtual void load(Framework::Scene::Collecter& collecter) override;
    /**
    * @brief �X�V
    */
    virtual void update() override;
    /**
    * @brief �I�����Ă��邩
    */
    virtual bool isEndScene() const override;
    /**
    * @brief �`��
    */
    virtual void draw() override;
    /**
    * @brief �I������
    */
    virtual void end() override;
    /**
    * @brief ���̃V�[��
    */
    virtual Framework::Define::SceneType next() override;
private:
    std::unique_ptr<Framework::Graphics::VertexBuffer> mVertexBuffer; //!< ���_�o�b�t�@
    std::unique_ptr<Framework::Graphics::IndexBuffer> mIndexBuffer; //!< �C���f�b�N�X�o�b�t�@
    std::unique_ptr<Framework::Graphics::Pipeline> mPipeline;
    std::shared_ptr<Framework::Graphics::Texture> mTexture;
    Framework::Graphics::MVPCBuffer mMVP;
    float mRotate;
};
