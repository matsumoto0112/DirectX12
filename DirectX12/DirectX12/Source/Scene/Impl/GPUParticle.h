//#pragma once
//#include "DirectX12/Source/stdafx.h"
//#include "Framework/Utility/Typedef.h"
//#include "Framework/Scene/SceneBase.h"
//#include "Framework/Math/Vector4.h"
//#include "Framework/Graphics/Color4.h"
//#include "Framework/Math/Matrix4x4.h"
//
///**
//* @class GPUParticle
//* @brief discription
//*/
//class GPUParticle : public Framework::Scene::SceneBase {
//    struct Particle {
//        float lifeTime;
//        float speed;
//        Framework::Math::Vector3 pos;
//        Framework::Math::Vector3 vel;
//        Framework::Graphics::Color4 color;
//    };
//
//    struct MVP {
//        Framework::Math::Matrix4x4 world;
//        Framework::Math::Matrix4x4 view;
//        Framework::Math::Matrix4x4 proj;
//    };
//
//    /**
//    * @enum GraphicsRootParameter
//    * @brief description
//    */
//    enum  GraphicsRootParameter {
//        Cbv,
//        GraphicsRootParametersCount,
//    };
//
//    /**
//    * @enum ComputeRootParameters
//    * @brief description
//    */
//    enum  ComputeRootParameters {
//        UavTable,
//        ComputeRootParametersCount,
//    };
//
//    /**
//    * @enum HeapOffsets
//    * @brief description
//    */
//    enum  HeapOffsets {
//        CbvSrvOffset = 0,
//        CommandsOffset = CbvSrvOffset + 1,
//        ProcessedCommandsOffset = CommandsOffset + 1,
//        CbvSrvUavDescriptorCountPerFrame = ProcessedCommandsOffset + 1,
//    };
//
//    //UAVのアラインメント
//    static inline UINT alignForUavCounter(UINT bufferSize) {
//        const UINT aligmment = D3D12_UAV_COUNTER_PLACEMENT_ALIGNMENT;
//        return (bufferSize + (aligmment - 1)) & ~(aligmment - 1);
//    }
//
//public:
//    /**
//    * @brief コンストラクタ
//    */
//    GPUParticle(HWND hWnd);
//    /**
//    * @brief デストラクタ
//    */
//    ~GPUParticle();
//    /**
//    * @brief シーンデータの読み込み
//    */
//    virtual void load(Framework::Scene::Collecter& collecter) override;
//    /**
//    * @brief 更新
//    */
//    virtual void update() override;
//    /**
//    * @brief 終了しているか
//    */
//    virtual bool isEndScene() const override;
//    /**
//    * @brief 描画
//    */
//    virtual void draw() override;
//    /**
//    * @brief 終了処理
//    */
//    virtual void end() override;
//    /**
//    * @brief 次のシーン
//    */
//    virtual Framework::Define::SceneType next() override;
//private:
//    static constexpr UINT FRAME_COUNT = 3;
//    static constexpr UINT THREAD_X = 128;
//    static constexpr UINT THREAD_Y = 128;
//    static constexpr UINT DISPATCH_X = 1;
//    static constexpr UINT DISPATCH_Y = 1;
//    static constexpr UINT PARTICLE_NUM = THREAD_X * THREAD_Y * DISPATCH_X * DISPATCH_Y;
//
//    std::vector<Particle> mConstantBufferData; //!< コンスタントバッファデータ
//
//    MVP mMVPCBufferData;
//
//    //パイプライン
//    CD3DX12_VIEWPORT mViewport;
//    CD3DX12_RECT mScissorRect;
//    D3D12_RECT mCullingScissorRect;
//    ComPtr<IDXGISwapChain3> mSwapChain;
//    ComPtr<ID3D12Device> mDevice;
//    ComPtr<ID3D12Resource> mRenderTargets[FRAME_COUNT];
//    ComPtr<ID3D12CommandAllocator> mCommandAllocators[FRAME_COUNT];
//    ComPtr<ID3D12CommandAllocator> mComputeCommandAllocators; //!< コンピュートシェーダ―用アロケーター
//    ComPtr<ID3D12CommandQueue> mCommandQueue;
//    ComPtr<ID3D12CommandQueue> mComputeCommandQueue; //!< コンピュートシェーダー用キュー
//    ComPtr<ID3D12RootSignature> mRootSignature;
//    ComPtr<ID3D12RootSignature> mComputeRootSignature; //!< コンピュートシェーダ―用ルートシグネチャ
//    ComPtr<ID3D12DescriptorHeap> mRTVHeap;
//    ComPtr<ID3D12DescriptorHeap> mDSVHeap;
//    ComPtr<ID3D12DescriptorHeap> mParticleUAVHeap;
//    UINT mRTVDescriptorSize;
//    UINT mParticleUAVDescriptorSize;
//    UINT mFrameIndex;
//
//    //同期オブジェクト
//    ComPtr<ID3D12Fence> mFence;
//    ComPtr<ID3D12Fence> mComputeFence;
//    UINT64 mFenceValues[FRAME_COUNT];
//    HANDLE mFenceEvent;
//
//    //アセット
//    ComPtr<ID3D12PipelineState> mPipelineState;
//    ComPtr<ID3D12PipelineState> mComputePipelineState; //!< コンピュートシェーダ―用パイプライン
//    ComPtr<ID3D12GraphicsCommandList> mCommandList;
//    ComPtr<ID3D12GraphicsCommandList> mComputeCommandList; //!< コンピュートシェーダ―用コマンドリスト
//    ComPtr<ID3D12Resource> mConstantBuffer;
//    ComPtr<ID3D12Resource> mDepthStencil;
//    ComPtr<ID3D12Resource> mCommandBuffer;
//    ComPtr<ID3D12Resource> mParticleUAVBuffer;
//    D3D12_VERTEX_BUFFER_VIEW mVertexBufferView;
//
//    UINT mWidth;
//    UINT mHeight;
//    HWND mHWnd;
//
//    void loadPipeline();
//    void loadAssets();
//    float getRandomFloat(float min, float max);
//    void populateCommandList();
//    void waitForGPU();
//    void moveToNextFrame();
//
//    ComPtr<ID3D12DescriptorHeap> mImGUIDescriptorSrvHeap;
//};
