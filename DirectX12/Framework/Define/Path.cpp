#include "Path.h"
#include <Windows.h>
#include <iostream>

namespace {
static constexpr int PATH_MAX = 1024;
}

namespace Framework {
namespace Define {
Path::Path() {
    char path[PATH_MAX];
    DWORD ret = GetModuleFileName(nullptr, path, sizeof(path));
    mExe = std::string(path);

    char dir[PATH_MAX];
    char drive[PATH_MAX];
    char name[PATH_MAX];
    char ext[PATH_MAX];

    _splitpath_s(&mExe[0], drive, sizeof(drive), dir, sizeof(dir), name, sizeof(name), ext, sizeof(ext));
    std::string sDir(drive);
    sDir += dir;
    std::string::size_type pos = sDir.find_last_of("\\/");
    sDir = sDir.substr(0, pos);

    mShader = sDir + "/cso/Resources/Shader/";
    mResource = sDir + "/Resources/";
    mTexture = mResource + "Textures/";
    mFBX = mResource + "FBX/";
}

Path::~Path() { }

} //Define
} //Framework 
