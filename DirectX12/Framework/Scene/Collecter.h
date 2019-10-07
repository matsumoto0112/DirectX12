#pragma once
#include <memory>
#include <unordered_map>
#include "Framework/Define/SceneType.h"
#include "Framework/Scene/SceneDataBase.h"

namespace Framework {
namespace Scene {

/**
* @class Collecter
* @brief シーンコレクタクラス
*/
class Collecter {
private:
    using DataPtr = std::unique_ptr<SceneDataBase>;
    using DataContainer = std::unordered_map<Define::SceneType, DataPtr>;
public:
    /**
    * @brief コンストラクタ
    */
    Collecter();
    /**
    * @brief デストラクタ
    */
    ~Collecter();

    const SceneDataBase& getData(Define::SceneType type) const;
    template<class T>
    void setData(Define::SceneType type, const T& data);
private:
    DataContainer mDataContainer;
};
template<class T>
inline void Collecter::setData(Define::SceneType type, const T& data) {
    if (mDataContainer.find(type) == mDataContainer.end()) {
        mDataContainer[type] = std::make_unique<T>();
    }
    *mDataContainer[type] = data;
}
} //Scene 
} //Framework 
