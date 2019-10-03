#include "VertexBuffer.h"
#include "Framework/Graphics/DX12/DXWrap.h"

namespace Framework {
namespace Graphics {

VertexBuffer::~VertexBuffer() { }

void VertexBuffer::setCommand(ID3D12GraphicsCommandList* commandList) {
    commandList->IASetVertexBuffers(0, 1, &mVertexBufferView);
}

} //Graphics 
} //Framework 
