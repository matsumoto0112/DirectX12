#include "VertexBuffer.h"

namespace Framework {
namespace Graphics {

VertexBuffer::~VertexBuffer() { }

void VertexBuffer::addToCommandList(ID3D12GraphicsCommandList* commandList) const {
    commandList->IASetVertexBuffers(0, 1, &mVertexBufferView);
}

} //Graphics 
} //Framework 
