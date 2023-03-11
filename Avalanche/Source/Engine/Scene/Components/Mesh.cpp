#include "Mesh.h"

#include <glm/glm.hpp>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 color;
};

Mesh::Mesh(const std::string& meshPath)
{
    const std::vector<Vertex> vertices = {
        {{-0.5, 0.5, 0.0}, {1.0, 0.0, 0.0}},
        {{0.5, 0.5, 0.0}, {0.0, 1.0, 0.0}},
        {{0.5, -0.5, 0.0}, {0.0, 0.0, 1.0}},
        {{-0.5, -0.5, 0.0}, {1.0, 0.0, 0.0}},
    };

    const std::vector<uint32_t> indices = {
        0, 1, 2, 2, 3, 0
    };


    m_VertexBuffer = std::make_unique<VertexBuffer>(vertices.data(), vertices.size() * sizeof(Vertex));
    m_VertexBuffer->SetLayout({
        {ShaderDataType::Float3, "positions"},
        {ShaderDataType::Float3, "colors"}
    });

    m_IndexBuffer = std::make_unique<IndexBuffer>(indices.data(), indices.size());
}

void Mesh::Bind(vk::CommandBuffer commandBuffer) const
{
    commandBuffer.bindVertexBuffers(0, m_VertexBuffer->GetBuffer(), {0});
    commandBuffer.bindIndexBuffer(m_IndexBuffer->GetBuffer(), 0, vk::IndexType::eUint32);
}

void Mesh::Draw(vk::CommandBuffer commandBuffer) const
{
    commandBuffer.drawIndexed(m_IndexBuffer->GetCount(), 1, 0, 0, 0);
}
