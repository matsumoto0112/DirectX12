#include "Game.h"

#include <Windows.h>
#include "Framework/Device/GameDevice.h"
#include "Framework/Input/InputManager.h"
#include "Framework/Input/Mouse.h"
#include "Framework/Window/Window.h"
#include "Framework/Utility/Time.h"
#include "Framework/Define/Config.h"

namespace Framework {

Game::Game(const std::string& title, HINSTANCE hInstance) {
    Device::GameDevice::getInstance().initialize(title, hInstance);
}

Game::~Game() { }

int Game::run() {
    if (!init()) {
        return -1;
    }

    Device::GameDevice& device = Device::GameDevice::getInstance();
    //�^�C�}�[�̏������������ł���
    //���\�[�X�̓ǂݍ��݂ȂǂŎ��Ԃ��������Ă��邽��
    Utility::Time::getInstance().init(Define::Config::getInstance().FPS);
    Utility::Time::getInstance().startFrame();
    Utility::Time::getInstance().endFrame();

    MSG msg = {};

    //FPS��ۂݒ�Ȃ猈�܂������Ԃ܂őҋ@����悤�ɂ���
    const bool waitFlag = Define::Config::getInstance().keepFPS;

    //���C�����[�v
    while (true) {
        if (!isRunning()) {
            break;
        }
        if (device.isEnd()) {
            break;
        }
        //���b�Z�[�W������Ώ�������
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            Utility::Time::getInstance().startFrame();
            update();
            draw();
            Utility::Time::getInstance().endFrame();
            if (waitFlag) {
                Utility::Time::getInstance().wait();
            }
        }
    }

    finalize();
    return 0;
}

bool Game::init() {
    return true;
}

void Game::finalize() {
    Device::GameDevice::getInstance().finalize();
}

void Game::update() {
    Device::GameDevice::getInstance().update();
}

bool Game::isRunning() {
    std::vector<Input::KeyCode>& keys = Define::Config::getInstance().gameEndKeys;
    for (auto&& key : keys) {
        if (Device::GameDevice::getInstance().getInputManager()->getKeyboard().getKeyDown(key)) {
            return false;
        }
    }
    return true;
}

} //Framework 
