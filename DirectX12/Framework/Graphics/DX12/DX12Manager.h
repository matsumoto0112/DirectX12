#pragma once
#include <d3d12.h>
#include <dxgi1_4.h>

#include "Framework/Utility/Singleton.h"
#include "Framework/Utility/Typedef.h"

namespace Framework {
namespace Graphics {

/**
* @class DX12Manager
* @brief DirectX12管理クラス
*/
class DX12Manager :public Utility::Singleton<DX12Manager> {
public:
    /**
    * @brief コンストラクタ
    */
    DX12Manager();
    /**
    * @brief デストラクタ
    */
    ~DX12Manager();
    /**
    * @brief 初期化
    * @param hWnd ウィンドウハンドル
    * @param width ウィンドウの幅
    * @param height ウィンドウの高さ
    */
    void initialize(HWND hWnd, UINT width, UINT height);
    void finalize();
    void createFence();
    /**
    * @brief デバイスを取得する
    */
    ID3D12Device* getDevice() const { return mDevice.Get(); }
    ID3D12GraphicsCommandList* getCommandList() const { return mCommandList.Get(); }
    void drawBegin();

    void drawEnd();

    void executeCommand();
private:
    void waitForPreviousFrame();
private:
    static constexpr UINT FRAME_COUNT = 2;
    ComPtr<ID3D12Device> mDevice; //!< デバイス
    ComPtr<ID3D12CommandQueue> mCommandQueue; //!< コマンドキュー
    ComPtr<IDXGISwapChain3> mSwapChain; //!< スワップチェイン
    UINT mFrameIndex; //!< 現在のバックバッファフレーム番号
    ComPtr<ID3D12CommandAllocator> mCommandAllocator; //!< コマンドアロケータ
    ComPtr<ID3D12Resource> mRenderTargets[FRAME_COUNT]; //!< レンダーターゲット
    ComPtr<ID3D12DescriptorHeap> mRTVHeap; //!< RTV用
    ComPtr<ID3D12GraphicsCommandList> mCommandList; //!< コマンドリスト
    ComPtr<ID3D12Fence> mFence; //!< フェンス
    UINT64 mFenceValue;
    HANDLE mFenceEvent;
    UINT mRTVDescriptorSize; //!< RTVディスクリプタヒープの大きさ
    D3D12_VIEWPORT mViewport;
    D3D12_RECT mScissorRect;
    ComPtr<ID3D12RootSignature> mRootSignature; //!< ルートシグネチャ
    ComPtr<ID3D12PipelineState> mPipelineState; //!< パイプラインステート

};

} //Graphics 
} //Framework 