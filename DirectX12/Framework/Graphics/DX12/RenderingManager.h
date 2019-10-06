#pragma once
#include "Framework/Utility/Singleton.h"
#include "Framework/Graphics/DX12/DX12Manager.h"
#include "Framework/Graphics/DX12/Resource/ConstantBufferManager.h"
#include "Framework/Graphics/DX12/Resource/SRVManager.h"

namespace Framework {
namespace Graphics {

/**
* @class RenderingManager
* @brief 描画管理者
*/
class RenderingManager : public Utility::Singleton<RenderingManager> {
public:
    /**
    * @brief 初期化処理
    */
    void init(HWND hWnd, UINT width, UINT height);

    void begin();
    void end();

    DX12Manager* getDX12Manager() const { return mManager.get(); }

    ConstantBufferManager* getConstantBufferManager() const { return mConstantBufferManager.get(); }

    SRVManager* getSRVManager() const { return mSRVManager.get(); }
protected:
    /**
    * @brief コンストラクタ
    */
    RenderingManager();
    /**
    * @brief デストラクタ
    */
    ~RenderingManager();
private:
    std::unique_ptr<DX12Manager> mManager;
    std::unique_ptr<ConstantBufferManager> mConstantBufferManager;
    std::unique_ptr<SRVManager> mSRVManager;
};

} //Graphics 
} //Framework 