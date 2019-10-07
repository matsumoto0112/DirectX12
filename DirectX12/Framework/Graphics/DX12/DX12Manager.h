#pragma once
#include <d3d12.h>
#include <dxgi1_4.h>
#include "Framework/Utility/Typedef.h"
#include "Framework/Graphics/DX12/Render/RootSignature.h"
#include "Framework/Graphics/DX12/Render/Pipeline.h"

namespace Framework {
namespace Graphics {

/**
* @class DX12Manager
* @brief DirectX12管理クラス
*/
class DX12Manager {
public:
    /**
    * @brief コンストラクタ
    * @param hWnd ウィンドウハンドル
    * @param width ウィンドウの幅
    * @param height ウィンドウの高さ
    */
    DX12Manager(HWND hWnd, UINT width, UINT height);
    /**
    * @brief デストラクタ
    */
    ~DX12Manager();
    /**
    * @brief デフォルトのパイプライン作成
    */
    void createPipeline();
    /**
    * @brief デバイスを取得する
    */
    ID3D12Device* getDevice() const { return mDevice.Get(); }
    /**
    * @brief コマンドリストを取得する
    */
    ID3D12GraphicsCommandList* getCommandList() const { return mCommandList.Get(); }
    /**
    * @brief 描画開始
    */
    void drawBegin();
    /**
    * @brief 描画終了
    */
    void drawEnd();
    /**
    * @brief コマンドリストを実行する
    */
    void executeCommand();
    //private:
    /**
    * @brief フレーム経過まで待機する
    */
    void waitForPreviousFrame();
    /**
    * @brief メインとなるルートシグネチャの取得
    */
    std::shared_ptr<RootSignature> getMainRootSignature() const { return mRootSignature; }
private:
    static constexpr UINT FRAME_COUNT = 2;
    ComPtr<ID3D12Device> mDevice; //!< デバイス
    ComPtr<ID3D12CommandQueue> mCommandQueue; //!< コマンドキュー
    ComPtr<IDXGISwapChain3> mSwapChain; //!< スワップチェイン
    UINT mFrameIndex; //!< 現在のバックバッファフレーム番号
    ComPtr<ID3D12CommandAllocator> mCommandAllocator[FRAME_COUNT]; //!< コマンドアロケータ
    ComPtr<ID3D12Resource> mRenderTargets[FRAME_COUNT]; //!< レンダーターゲット
    ComPtr<ID3D12DescriptorHeap> mRTVHeap; //!< RTV用
    ComPtr<ID3D12GraphicsCommandList> mCommandList; //!< コマンドリスト
    ComPtr<ID3D12Fence> mFence; //!< フェンス
    UINT64 mFenceValue[FRAME_COUNT];
    HANDLE mFenceEvent;
    UINT mRTVDescriptorSize; //!< RTVディスクリプタヒープの大きさ
    D3D12_VIEWPORT mViewport;
    D3D12_RECT mScissorRect;
    std::unique_ptr<Pipeline> mDefaultPipeline;
    std::shared_ptr<RootSignature> mRootSignature;
};

} //Graphics 
} //Framework 