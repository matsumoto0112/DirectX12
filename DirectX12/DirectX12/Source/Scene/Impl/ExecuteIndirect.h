#pragma once
#include "DirectX12/Source/stdafx.h"
#include "Framework/Utility/Typedef.h"
#include "Framework/Scene/SceneBase.h"
#include "Framework/Math/Vector4.h"
#include "Framework/Graphics/Color4.h"
#include "Framework/Math/Matrix4x4.h"

struct Vertex {
    DirectX::XMFLOAT3 position;
};

struct SceneConstantBuffer {
    Framework::Math::Vector4 velocity;
    Framework::Math::Vector4 offset;
    Framework::Graphics::Color4 color;
    Framework::Math::Matrix4x4 view;
    Framework::Math::Matrix4x4 projection;

    float dummy[36];
};

struct CSRootConstants {
    float xOffset;
    float zOffset;
    float cullOffset;
    float commandCount;
};

struct IndirectCommand {
    D3D12_GPU_VIRTUAL_ADDRESS cbv;
    D3D12_DRAW_ARGUMENTS drawArguments;
};

/**
* @enum GraphicsRootParameter
* @brief description
*/
enum  GraphicsRootParameter {
    Cbv,
    GraphicsRootParametersCount,
};

/**
* @enum ComputeRootParameters
* @brief description
*/
enum  ComputeRootParameters {
    SrvUavTable,
    RootConstants,
    ComputeRootParametersCount,
};

/**
* @enum HeapOffsets
* @brief description
*/
enum  HeapOffsets {
    CbvSrvOffset = 0,
    CommandsOffset = CbvSrvOffset + 1,
    ProcessedCommandsOffset = CommandsOffset + 1,
    CbvSrvUavDescriptorCountPerFrame = ProcessedCommandsOffset + 1,
};

//UAV�̃A���C�������g
static inline UINT alignForUavCounter(UINT bufferSize) {
    const UINT aligmment = D3D12_UAV_COUNTER_PLACEMENT_ALIGNMENT;
    return (bufferSize + (aligmment - 1)) & ~(aligmment - 1);
}

/**
* @class ExecuteIndirect
* @brief discription
*/
class ExecuteIndirect : public Framework::Scene::SceneBase {
public:
    /**
    * @brief �R���X�g���N�^
    */
    ExecuteIndirect(HWND hWnd);
    /**
    * @brief �f�X�g���N�^
    */
    ~ExecuteIndirect();
    /**
    * @brief �V�[���f�[�^�̓ǂݍ���
    */
    virtual void load(Framework::Scene::Collecter& collecter) override;
    /**
    * @brief �X�V
    */
    virtual void update() override;
    /**
    * @brief �I�����Ă��邩
    */
    virtual bool isEndScene() const override;
    /**
    * @brief �`��
    */
    virtual void draw() override;
    /**
    * @brief �I������
    */
    virtual void end() override;
    /**
    * @brief ���̃V�[��
    */
    virtual Framework::Define::SceneType next() override;
private:
    static constexpr UINT FRAME_COUNT = 3;
    static constexpr UINT TRIANGLE_COUNT = 1024;
    static constexpr UINT TRIANGLE_RESOURCE_COUNT = TRIANGLE_COUNT * FRAME_COUNT;
    static const UINT COMMAND_SIZE_PER_FRAME;
    static const UINT COMMAND_BUFFER_COUNTER_OFFSET;
    static constexpr UINT COMPUTE_THREAD_BLOCK_SIZE = 128;
    static const float TRIANGLE_HALF_WIDTH;
    static const float TRIANGLE_DEPTH;
    static const float CULLING_CUT_OFF;

    //���̃f�[�^�����ׂĂ̎O�p�`���Q�Ƃ���
    std::vector<SceneConstantBuffer> mConstantBufferData; //!< �R���X�^���g�o�b�t�@�f�[�^
    UINT8* mCbvDataBegin;

    //�R���s���[�g�V�F�[�_�[�p�o�b�t�@
    CSRootConstants mCSRootConstants;
    bool mEnableCulling;

    //�p�C�v���C��
    CD3DX12_VIEWPORT mViewport;
    CD3DX12_RECT mScissorRect;
    D3D12_RECT mCullingScissorRect;
    ComPtr<IDXGISwapChain3> mSwapChain;
    ComPtr<ID3D12Device> mDevice;
    ComPtr<ID3D12Resource> mRenderTargets[FRAME_COUNT];
    ComPtr<ID3D12CommandAllocator> mCommandAllocators[FRAME_COUNT];
    ComPtr<ID3D12CommandAllocator> mComputeCommandAllocators[FRAME_COUNT];
    ComPtr<ID3D12CommandQueue> mCommandQueue;
    ComPtr<ID3D12CommandQueue> mComputeCommandQueue;
    ComPtr<ID3D12RootSignature> mRootSignature;
    ComPtr<ID3D12RootSignature> mComputeRootSignature;
    ComPtr<ID3D12CommandSignature> mCommandSignature;
    ComPtr<ID3D12DescriptorHeap> mRTVHeap;
    ComPtr<ID3D12DescriptorHeap> mDSVHeap;
    ComPtr<ID3D12DescriptorHeap> mCBV_SRV_UAV_Heap;
    UINT mRTVDescriptorSize;
    UINT mCBV_SRV_UAV_DescriptorSize;
    UINT mFrameIndex;

    //�����I�u�W�F�N�g
    ComPtr<ID3D12Fence> mFence;
    ComPtr<ID3D12Fence> mComputeFence;
    UINT64 mFenceValues[FRAME_COUNT];
    HANDLE mFenceEvent;

    //�A�Z�b�g
    ComPtr<ID3D12PipelineState> mPipelineState;
    ComPtr<ID3D12PipelineState> mComputeState;
    ComPtr<ID3D12GraphicsCommandList> mCommandList;
    ComPtr<ID3D12GraphicsCommandList> mComputeCommandList;
    ComPtr<ID3D12Resource> mVertexBuffer;
    ComPtr<ID3D12Resource> mConstantBuffer;
    ComPtr<ID3D12Resource> mDepthStencil;
    ComPtr<ID3D12Resource> mCommandBuffer;
    ComPtr<ID3D12Resource> mProcessedCommandBuffers[FRAME_COUNT];
    ComPtr<ID3D12Resource> mProcessedCommandBufferCounterReset;
    D3D12_VERTEX_BUFFER_VIEW mVertexBufferView;

    UINT mWidth;
    UINT mHeight;
    HWND mHWnd;

    void loadPipeline();
    void loadAssets();
    float getRandomFloat(float min, float max);
    void populateCommandList();
    void waitForGPU();
    void moveToNextFrame();
};
