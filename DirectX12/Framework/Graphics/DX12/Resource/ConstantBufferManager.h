#pragma once
#include <vector>
#include <memory>
#include <d3d12.h>
#include "Framework/Graphics/DX12/Resource/ConstantBuffer.h"
#include "Framework/Utility/Singleton.h"

namespace Framework {
namespace Graphics {
class ConstantBuffer;

/**
* @class ConstantBufferManager
* @brief �R���X�^���g�o�b�t�@�Ǘ�
*/
class ConstantBufferManager {
public:
    /**
    * @brief �R���X�g���N�^
    */
    ConstantBufferManager();
    /**
    * @brief �f�X�g���N�^
    */
    ~ConstantBufferManager();
    /**
    * @brief �`��J�n���ɌĂ�
    */
    void beginFrame();

    void beingCBufferUpdate();
    void endCBufferUpdate(ID3D12GraphicsCommandList* commandList);
    /**
    * @brief �o�b�t�@���X�V����
    */
    template <class T>
    void updateCBuffer(const T& data);
private:
    /**
    * @brief �R���X�^���g�o�b�t�@��ǉ�����
    */
    void addConstantBuffer();
private:
    static constexpr UINT CONSTANT_BUFFER_RESERVATION_NUM = 100; //!< �R���X�^���g�o�b�t�@�̃������\��
    std::vector<std::unique_ptr<ConstantBuffer>> mConstantBuffers; //!< �R���X�^���g�o�b�t�@
    UINT mCurrentIndex; //!< ���݂̃R���X�^���g�o�b�t�@�C���f�b�N�X
    std::vector<UINT> mBeginedIndex;
};

template<class T>
inline void ConstantBufferManager::updateCBuffer(const T& data) {
    //�X�V�\�ȗ̈悪����Ȃ畁�ʂɍX�V����
    if (mConstantBuffers[mCurrentIndex]->canUpdate(sizeof(data))) {
        mConstantBuffers[mCurrentIndex]->updateBuffer((void*)&data, sizeof(T));
    }
    else {
        //�_���������玟�̃o�b�t�@�Ŏ���
        mCurrentIndex++;
        mBeginedIndex.emplace_back(mCurrentIndex);
        //���̃o�b�t�@���Ȃ���Βǉ�����
        if (mCurrentIndex >= mConstantBuffers.size()) {
            addConstantBuffer();
        }
        updateCBuffer(data);
    }
}

} //Graphics 
} //Framework 