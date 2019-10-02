#pragma once

#include "Framework/Math/Vector2.h"

namespace Framework {
namespace Device {
class GameDevice;
} //Device 

/**
* @class Game
* @brief �Q�[���N���X
*/
class Game {
protected:
    /**
    * @brief �R���X�g���N�^
    * @param title �Q�[���̃^�C�g��
    */
    Game(const std::string& title, HINSTANCE hInstance);
    /**
    * @brief �f�X�g���N�^
    */
    virtual ~Game();
public:
    /**
    * @brief ���s
    */
    int run();
    /**
    * @brief ������
    * @return ����������true��Ԃ�
    */
    virtual bool init();
    /**
    * @brief �I������
    */
    virtual void finalize();
protected:
    /**
    * @brief �X�V
    */
    virtual void update();
    /**
    * @brief �`��
    */
    virtual void draw() = 0;
    /**
    * @brief ���s�����H
    */
    virtual bool isRunning();
private:
    //�R�s�[�֎~
    Game(const Game& other) = delete;
    Game& operator=(const Game& other) = delete;
};

} //Framework 