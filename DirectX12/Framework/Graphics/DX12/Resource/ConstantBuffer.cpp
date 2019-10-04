#include "ConstantBuffer.h"

namespace Framework {
namespace Graphics {

ConstantBuffer::~ConstantBuffer() { }

void ConstantBuffer::addToCommandList(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex) {
    commandList->SetGraphicsRootConstantBufferView(rootParameterIndex, mConstantBuffer->GetGPUVirtualAddress());
}

} //Graphics 
} //Framework 
