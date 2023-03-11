#pragma once

#include "Engine/Vulkan/Buffers.h"

class Mesh
{
public:
    Mesh(const std::string& meshPath);

    void Bind(vk::CommandBuffer commandBuffer) const;
    void Draw(vk::CommandBuffer commandBuffer) const;
    VertexBuffer& GetVertexBuffer() const { return *m_VertexBuffer; }
    IndexBuffer& GetIndexBuffer() const { return *m_IndexBuffer; }

private:
    std::unique_ptr<VertexBuffer> m_VertexBuffer;
    std::unique_ptr<IndexBuffer> m_IndexBuffer;
};
