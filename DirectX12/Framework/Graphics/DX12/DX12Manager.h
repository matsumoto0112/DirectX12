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
* @brief DirectX12�̕`�������Ǘ�����
*/
class DX12Manager : public Utility::Singleton<DX12Manager> {
public:
    /**
    * @brief ����������
    */
    void init(HWND hWnd, UINT width, UINT height);
    /**
    * @brief �f�o�C�X�̎擾
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
    * @brief �R���X�g���N�^
    */
    DX12Manager();
    /**
    * @brief �f�X�g���N�^
    */
    ~DX12Manager();
//private:
    void waitForPreviousFrame();
    void barrier(ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);
//private:
    static constexpr UINT FrameCount = 2;
    UINT mFrameIndex; //!< ���݂̃t���[���C���f�b�N�X
    UINT mRTVDescriptorSize;
    ComPtr<ID3D12Device> mDevice; //!< �f�o�C�X
    ComPtr<IDXGISwapChain3> mSwapChain; //!< �X���b�v�`�F�C��
    ComPtr<ID3D12CommandQueue> mCommandQueue; //!< �R�}���h�L���[
    ComPtr<ID3D12Fence> mQueueFence; //!< �L���[�̃t�F���X
    HANDLE mFenceEvent; //!< �C�x���g�t�F���X�n���h��
    ComPtr<ID3D12DescriptorHeap> mDescriptorHeap; //!< �����_�[�^�[�Q�b�g�̃q�[�v
    ComPtr<ID3D12Resource> mRenderTarget[FrameCount]; //!< �����_�[�^�[�Q�b�g
    D3D12_CPU_DESCRIPTOR_HANDLE mRTVHandle[FrameCount]; //!<�����_�[�^�[�Q�b�g�n���h��
    ComPtr<ID3D12CommandAllocator> mCommandAllocator; //!< �R�}���h�A���P�[�^
    ComPtr<ID3D12GraphicsCommandList> mCommandList; //!< �R�}���h���X�g
    std::unique_ptr<Pipeline> mDefaultPipeline; //!< �f�t�H���g�̃p�C�v���C��
    Pipeline* mCurrentPipeline; //!< ���ݐݒ蒆�̃p�C�v���C��
    D3D12_VIEWPORT mViewport;
    D3D12_RECT mScissorRect;
};

} //Graphics 
} //Framework 