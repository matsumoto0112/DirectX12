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
    MY_ASSERTION(mCurrentScene, "ƒV[ƒ“‚ª‘¶İ‚µ‚Ü‚¹‚ñ");
    mCurrentScene->update();
    if (mCurrentScene->isEndScene()) {
        loadScene(mCurrentScene->next());
    }
}

void Manager::draw() {
    MY_ASSERTION(mCurrentScene, "ƒV[ƒ“‚ª‘¶İ‚µ‚Ü‚¹‚ñ");
    mCurrentScene->draw();
}

void Manager::registerScene(Define::SceneType type, ScenePtr scene) {
    MY_DEBUG_LOG_IF(mCreatedScene.find(type) != mCreatedScene.end(), "“o˜^Ï‚İ‚ÌƒV[ƒ“‚ªÄ“o˜^‚³‚ê‚Ü‚µ‚½");
    mCreatedScene[type] = std::move(scene);
}

void Manager::loadScene(Define::SceneType next) {
    if (mCurrentScene)mCurrentScene->end();
    mCurrentScene = getNextScene(next);
    mCurrentScene->load(*mCollecter);
}

SceneBase* Manager::getNextScene(Define::SceneType next) {
    MY_ASSERTION(mCreatedScene.find(next) != mCreatedScene.end(), "–¢“o˜^‚ÌƒV[ƒ“‚ªŒÄ‚Î‚ê‚Ü‚µ‚½");
    return mCreatedScene[next].get();
}

} //Scene 
} //Framework 

