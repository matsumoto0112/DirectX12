#include "SRVManager.h"
#include "Framework/Graphics/DX12/DXInterfaceAccessor.h"
#include "Framework/Graphics/DX12/Helper.h"
#include "Framework/Graphics/DX12/Resource/ShaderResourceView.h"

namespace Framework {
namespace Graphics {

SRVManager::SRVManager()
    :mCurrentIndex(0) {
    mSRVs.emplace_back(std::make_unique<ShaderResourceView>(TEXTURE_RESERVATION_NUM));
}

SRVManager::~SRVManager() {
    mSRVs.clear();
    mBeginedIndex.clear();
}

void SRVManager::beginFrame() {
    mBeginedIndex.clear();
    mCurrentIndex = 0;
}

void SRVManager::beginUpdate() {
    mBeginedIndex.emplace_back(mCurrentIndex);
    for (auto&& srv : mSRVs) {
        srv->beginFrame();
    }
}

void SRVManager::endUpdate(ID3D12GraphicsCommandList* commandList) {
    for (auto&& i : mBeginedIndex) {
        mSRVs[i]->endCBUpdate(commandList);
    }
    mBeginedIndex.clear();
}

void SRVManager::setResource(const std::shared_ptr<Texture> texture) {
    if (mSRVs[mCurrentIndex]->canUpdate()) {
        mSRVs[mCurrentIndex]->updateBuffer(texture->mTexture);
    }
    else {
        mCurrentIndex++;
        mBeginedIndex.emplace_back(mCurrentIndex);
        setResource(texture);
    }
}

} //Graphics 
} //Framework 
