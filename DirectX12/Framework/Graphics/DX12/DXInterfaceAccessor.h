#pragma once
#include <d3d12.h>

namespace Framework {
namespace Graphics {

/**
* @class DXInterfaceAccessor
* @brief DirectXインターフェースへのアクセサ
*/
class DXInterfaceAccessor {
public:
    /**
    * @brief デバイスを取得する
    */
    static ID3D12Device* getDevice();
};

} //Graphics 
} //Framework 