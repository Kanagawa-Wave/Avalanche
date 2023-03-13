#pragma once

#include "Engine/Vulkan/Buffers.h"

#include <glm/glm.hpp>

struct Vertex
{
    glm::vec3 position{};
    glm::vec3 color{};
    glm::vec3 normal{};
    glm::vec2 uv{};

    bool operator ==(const Vertex& other) const
    {
        return
            position == other.position &&
            color == other.color &&
            normal == other.normal &&
            uv == other.uv;
    }

    static BufferLayout Layout()
    {
        return {
                {ShaderDataType::Float3, "position"},
                {ShaderDataType::Float3, "color"},
                {ShaderDataType::Float3, "normal"},
                {ShaderDataType::Float2, "uv"}
        };
    }
};

class Mesh
{
public:
    Mesh(const std::string& meshPath);

    void Bind(vk::CommandBuffer commandBuffer) const;
    void Draw(vk::CommandBuffer commandBuffer) const;
    
    VertexBuffer& GetVertexBuffer() const { return *m_VertexBuffer; }
    IndexBuffer& GetIndexBuffer() const { return *m_IndexBuffer; }

private:
    void LoadObjFromFile(const std::string& path);

    std::vector<Vertex> m_Vertices;
    std::vector<uint32_t> m_Indices;
    
    std::unique_ptr<VertexBuffer> m_VertexBuffer;
    std::unique_ptr<IndexBuffer> m_IndexBuffer;
};
