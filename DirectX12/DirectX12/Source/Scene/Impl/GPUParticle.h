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
//    //UAV�̃A���C�������g
//    static inline UINT alignForUavCounter(UINT bufferSize) {
//        const UINT aligmment = D3D12_UAV_COUNTER_PLACEMENT_ALIGNMENT;
//        return (bufferSize + (aligmment - 1)) & ~(aligmment - 1);
//    }
//
//public:
//    /**
//    * @brief �R���X�g���N�^
//    */
//    GPUParticle(HWND hWnd);
//    /**
//    * @brief �f�X�g���N�^
//    */
//    ~GPUParticle();
//    /**
//    * @brief �V�[���f�[�^�̓ǂݍ���
//    */
//    virtual void load(Framework::Scene::Collecter& collecter) override;
//    /**
//    * @brief �X�V
//    */
//    virtual void update() override;
//    /**
//    * @brief �I�����Ă��邩
//    */
//    virtual bool isEndScene() const override;
//    /**
//    * @brief �`��
//    */
//    virtual void draw() override;
//    /**
//    * @brief �I������
//    */
//    virtual void end() override;
//    /**
//    * @brief ���̃V�[��
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
//    std::vector<Particle> mConstantBufferData; //!< �R���X�^���g�o�b�t�@�f�[�^
//
//    MVP mMVPCBufferData;
//
//    //�p�C�v���C��
//    CD3DX12_VIEWPORT mViewport;
//    CD3DX12_RECT mScissorRect;
//    D3D12_RECT mCullingScissorRect;
//    ComPtr<IDXGISwapChain3> mSwapChain;
//    ComPtr<ID3D12Device> mDevice;
//    ComPtr<ID3D12Resource> mRenderTargets[FRAME_COUNT];
//    ComPtr<ID3D12CommandAllocator> mCommandAllocators[FRAME_COUNT];
//    ComPtr<ID3D12CommandAllocator> mComputeCommandAllocators; //!< �R���s���[�g�V�F�[�_�\�p�A���P�[�^�[
//    ComPtr<ID3D12CommandQueue> mCommandQueue;
//    ComPtr<ID3D12CommandQueue> mComputeCommandQueue; //!< �R���s���[�g�V�F�[�_�[�p�L���[
//    ComPtr<ID3D12RootSignature> mRootSignature;
//    ComPtr<ID3D12RootSignature> mComputeRootSignature; //!< �R���s���[�g�V�F�[�_�\�p���[�g�V�O�l�`��
//    ComPtr<ID3D12DescriptorHeap> mRTVHeap;
//    ComPtr<ID3D12DescriptorHeap> mDSVHeap;
//    ComPtr<ID3D12DescriptorHeap> mParticleUAVHeap;
//    UINT mRTVDescriptorSize;
//    UINT mParticleUAVDescriptorSize;
//    UINT mFrameIndex;
//
//    //�����I�u�W�F�N�g
//    ComPtr<ID3D12Fence> mFence;
//    ComPtr<ID3D12Fence> mComputeFence;
//    UINT64 mFenceValues[FRAME_COUNT];
//    HANDLE mFenceEvent;
//
//    //�A�Z�b�g
//    ComPtr<ID3D12PipelineState> mPipelineState;
//    ComPtr<ID3D12PipelineState> mComputePipelineState; //!< �R���s���[�g�V�F�[�_�\�p�p�C�v���C��
//    ComPtr<ID3D12GraphicsCommandList> mCommandList;
//    ComPtr<ID3D12GraphicsCommandList> mComputeCommandList; //!< �R���s���[�g�V�F�[�_�\�p�R�}���h���X�g
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