#include "Collecter.h"

namespace Framework {
namespace Scene {

Collecter::Collecter() {}

Collecter::~Collecter() {}

const SceneDataBase& Collecter::getData(Define::SceneType type) const {
    return *mDataContainer.find(type)->second;
}

} //Scene 
} //Framework 
