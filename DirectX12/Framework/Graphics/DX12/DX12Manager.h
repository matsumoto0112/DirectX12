#pragma once
#include <d3d12.h>
#include <dxgi1_4.h>
#include "Framework/Utility/Singleton.h"
#include "Framework/Utility/Typedef.h"
#include "Framework/Graphics/DX12/Pipeline.h"

namespace Framework {
namespace Graphics {

/**
* @class DX12Manager
* @brief DirectX12の描画周りを管理する
*/
class DX12Manager : public Utility::Singleton<DX12Manager> {
public:
    /**
    * @brief 初期化処理
    */
    void init(HWND hWnd, UINT width, UINT height);
    /**
    * @brief デバイスの取得
    */
    ID3D12Device* getDevice() const;
    void setPipeline(Pipeline* pipeline);
    void begin();
    void end();
    void finalize();
    ID3D12GraphicsCommandList* getCommandList() const;
    void updateSubResource(ComPtr<ID3D12Resource> resource, UINT firstSubResource, UINT subResourceCount, D3D12_SUBRESOURCE_DATA* subData);
//protected:
    /**
    * @brief コンストラクタ
    */
    DX12Manager();
    /**
    * @brief デストラクタ
    */
    ~DX12Manager();
//private:
    void waitForPreviousFrame();
    void barrier(ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);
//private:
    static constexpr UINT FrameCount = 2;
    UINT mFrameIndex; //!< 現在のフレームインデックス
    UINT mRTVDescriptorSize;
    ComPtr<ID3D12Device> mDevice; //!< デバイス
    ComPtr<IDXGISwapChain3> mSwapChain; //!< スワップチェイン
    ComPtr<ID3D12CommandQueue> mCommandQueue; //!< コマンドキュー
    ComPtr<ID3D12Fence> mQueueFence; //!< キューのフェンス
    HANDLE mFenceEvent; //!< イベントフェンスハンドル
    ComPtr<ID3D12DescriptorHeap> mDescriptorHeap; //!< レンダーターゲットのヒープ
    ComPtr<ID3D12Resource> mRenderTarget[FrameCount]; //!< レンダーターゲット
    D3D12_CPU_DESCRIPTOR_HANDLE mRTVHandle[FrameCount]; //!<レンダーターゲットハンドル
    ComPtr<ID3D12CommandAllocator> mCommandAllocator; //!< コマンドアロケータ
    ComPtr<ID3D12GraphicsCommandList> mCommandList; //!< コマンドリスト
    std::unique_ptr<Pipeline> mDefaultPipeline; //!< デフォルトのパイプライン
    Pipeline* mCurrentPipeline; //!< 現在設定中のパイプライン
    D3D12_VIEWPORT mViewport;
    D3D12_RECT mScissorRect;
};

} //Graphics 
} //Framework 