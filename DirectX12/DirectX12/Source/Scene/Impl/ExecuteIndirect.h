#pragma once
#include "Framework/Scene/SceneBase.h"

/**
* @class ExecuteIndirect
* @brief discription
*/
class ExecuteIndirect : public Framework::Scene::SceneBase {
public:
    /**
    * @brief �R���X�g���N�^
    */
    ExecuteIndirect();
    /**
    * @brief �f�X�g���N�^
    */
    ~ExecuteIndirect();
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
};
