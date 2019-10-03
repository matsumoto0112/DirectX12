#pragma once
#include <d3d12.h>
#include <dxgi1_4.h>

#include "Framework/Utility/Singleton.h"
#include "Framework/Utility/Typedef.h"

namespace Framework {
namespace Graphics {

/**
* @class DX12Manager
* @brief DirectX12�Ǘ��N���X
*/
class DX12Manager :public Utility::Singleton<DX12Manager> {
public:
    /**
    * @brief �R���X�g���N�^
    */
    DX12Manager();
    /**
    * @brief �f�X�g���N�^
    */
    ~DX12Manager();
    /**
    * @brief ������
    * @param hWnd �E�B���h�E�n���h��
    * @param width �E�B���h�E�̕�
    * @param height �E�B���h�E�̍���
    */
    void initialize(HWND hWnd, UINT width, UINT height);
    void finalize();
    void createFence();
    /**
    * @brief �f�o�C�X���擾����
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
    ComPtr<ID3D12Device> mDevice; //!< �f�o�C�X
    ComPtr<ID3D12CommandQueue> mCommandQueue; //!< �R�}���h�L���[
    ComPtr<IDXGISwapChain3> mSwapChain; //!< �X���b�v�`�F�C��
    UINT mFrameIndex; //!< ���݂̃o�b�N�o�b�t�@�t���[���ԍ�
    ComPtr<ID3D12CommandAllocator> mCommandAllocator; //!< �R�}���h�A���P�[�^
    ComPtr<ID3D12Resource> mRenderTargets[FRAME_COUNT]; //!< �����_�[�^�[�Q�b�g
    ComPtr<ID3D12DescriptorHeap> mRTVHeap; //!< RTV�p
    ComPtr<ID3D12GraphicsCommandList> mCommandList; //!< �R�}���h���X�g
    ComPtr<ID3D12Fence> mFence; //!< �t�F���X
    UINT64 mFenceValue;
    HANDLE mFenceEvent;
    UINT mRTVDescriptorSize; //!< RTV�f�B�X�N���v�^�q�[�v�̑傫��
    D3D12_VIEWPORT mViewport;
    D3D12_RECT mScissorRect;
    ComPtr<ID3D12RootSignature> mRootSignature; //!< ���[�g�V�O�l�`��
    ComPtr<ID3D12PipelineState> mPipelineState; //!< �p�C�v���C���X�e�[�g

};

} //Graphics 
} //Framework 