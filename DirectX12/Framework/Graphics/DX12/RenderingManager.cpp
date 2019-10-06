#include "RenderingManager.h"

namespace Framework {
namespace Graphics {
RenderingManager::RenderingManager() { }

RenderingManager::~RenderingManager() { }

void RenderingManager::init(HWND hWnd, UINT width, UINT height) {
    mManager = std::make_unique<DX12Manager>(hWnd, width, height);
    mManager->createPipeline();
    mConstantBufferManager = std::make_unique<ConstantBufferManager>();
    mSRVManager = std::make_unique<SRVManager>();
    mManager->executeCommand();
    mManager->waitForPreviousFrame();
}

void RenderingManager::begin() {
    mManager->drawBegin();
    mConstantBufferManager->beginFrame();
    mSRVManager->beginFrame();
}

void RenderingManager::end() {
    mManager->drawEnd();
}

} //Graphics 
} //Framework 
