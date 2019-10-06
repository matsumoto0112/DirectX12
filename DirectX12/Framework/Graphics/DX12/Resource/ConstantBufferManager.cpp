#include "ConstantBufferManager.h"

namespace Framework {
namespace Graphics {

ConstantBufferManager::ConstantBufferManager()
    :mCurrentIndex(0) {
    addConstantBuffer();
}

ConstantBufferManager::~ConstantBufferManager() {
    mConstantBuffers.clear();
}

void ConstantBufferManager::beginFrame() {
    mCurrentIndex = 0;
    mBeginedIndex.clear();
    for (auto&& cb : mConstantBuffers) {
        cb->beginFrame();
    }
}

void ConstantBufferManager::beingCBufferUpdate() {
    mConstantBuffers[mCurrentIndex]->beginCBUpdate();
    mBeginedIndex.emplace_back(mCurrentIndex);
}

void ConstantBufferManager::endCBufferUpdate(ID3D12GraphicsCommandList* commandList) {
    for (auto&& i : mBeginedIndex) {
        mConstantBuffers[i]->endCBUpdate(commandList);
    }
    mBeginedIndex.clear();
}

void ConstantBufferManager::addConstantBuffer() {
    mConstantBuffers.emplace_back(std::make_unique<ConstantBuffer>(CONSTANT_BUFFER_RESERVATION_NUM));
}


} //Graphics 
} //Framework 
