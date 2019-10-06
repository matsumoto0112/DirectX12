#pragma once
#include "Framework/Utility/Singleton.h"
#include "Framework/Graphics/DX12/DX12Manager.h"
#include "Framework/Graphics/DX12/Resource/ConstantBufferManager.h"

namespace Framework {
namespace Graphics {

/**
* @class RenderingManager
* @brief �`��Ǘ���
*/
class RenderingManager : public Utility::Singleton<RenderingManager> {
public:
    /**
    * @brief ����������
    */
    void init(HWND hWnd, UINT width, UINT height);

    void begin();
    void end();

    DX12Manager* getDX12Manager() const { return mManager.get(); }

    ConstantBufferManager* getConstantBufferManager() const { return mConstantBufferManager.get(); }
protected:
    /**
    * @brief �R���X�g���N�^
    */
    RenderingManager();
    /**
    * @brief �f�X�g���N�^
    */
    ~RenderingManager();
private:
    std::unique_ptr<DX12Manager> mManager;
    std::unique_ptr<ConstantBufferManager> mConstantBufferManager;
};

} //Graphics 
} //Framework 