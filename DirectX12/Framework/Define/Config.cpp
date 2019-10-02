#include "Config.h"


namespace Framework {
namespace Define {

Config::Config()
    :mScreenWidth(800),
    mScreenHeight(600),
    mKeepFPS(false),
    mFPS(60.0f),
    mGameEndKeys({ Input::KeyCode::Escape }) { }

Config::~Config() { }

} //Define 
} //Framework 
