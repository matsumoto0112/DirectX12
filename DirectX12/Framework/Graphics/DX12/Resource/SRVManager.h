#pragma once
#include <d3d12.h>
#include "Framework/Graphics/DX12/Resource/Texture.h"

namespace Framework {
namespace Graphics {

/**
* @class SRVManager
* @brief シェーダーリソースビュー管理
*/
class SRVManager {
public:
    /**
    * @brief コンストラクタ
    */
    SRVManager();
    /**
    * @brief デストラクタ
    */
    ~SRVManager();

    void beginFrame();

    void beginUpdate();

    void endUpdate();

    void setResource();
private:
    static constexpr UINT TEXTURE_RESERVATION_NUM = 10000;
    ComPtr<ID3D12DescriptorHeap> mSRVHeap;
};

} //Graphics 
} //Framework 