#include "Manager.h"
#include "Framework/Scene/Collecter.h"
#include "Framework/Scene/SceneBase.h"
#include "Framework/Utility/Debug.h"

namespace Framework {
namespace Scene {

Manager::Manager()
    :mCollecter(std::make_unique<Collecter>()) {}

Manager::~Manager() {}

void Manager::update() {
    MY_ASSERTION(mCurrentScene, "シーンが存在しません");
    mCurrentScene->update();
    if (mCurrentScene->isEndScene()) {
        loadScene(mCurrentScene->next());
    }
}

void Manager::draw() {
    MY_ASSERTION(mCurrentScene, "シーンが存在しません");
    mCurrentScene->draw();
}

void Manager::registerScene(Define::SceneType type, ScenePtr scene) {
    MY_DEBUG_LOG_IF(mCreatedScene.find(type) != mCreatedScene.end(), "登録済みのシーンが再登録されました");
    mCreatedScene[type] = std::move(scene);
}

void Manager::loadScene(Define::SceneType next) {
    if (mCurrentScene)mCurrentScene->end();
    mCurrentScene = getNextScene(next);
    mCurrentScene->load(*mCollecter);
}

SceneBase* Manager::getNextScene(Define::SceneType next) {
    MY_ASSERTION(mCreatedScene.find(next) != mCreatedScene.end(), "未登録のシーンが呼ばれました");
    return mCreatedScene[next].get();
}

} //Scene 
} //Framework 

