#pragma once

#include "Engine/Vulkan/API/Buffers.h"

#include <glm/glm.hpp>

#include "Engine/Vulkan/API/Texture.h"

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

    void AddTexture(const std::string& path);
    
    Texture* GetTexture() const { return m_Texture.get(); }
    VertexBuffer* GetVertexBuffer() const { return m_VertexBuffer.get(); }
    IndexBuffer* GetIndexBuffer() const { return m_IndexBuffer.get(); }

private:
    void LoadObjFromFile(const std::string& path);

    std::unique_ptr<Texture> m_Texture = nullptr;
    
    std::vector<Vertex> m_Vertices;
    std::vector<uint32_t> m_Indices;
    
    std::unique_ptr<VertexBuffer> m_VertexBuffer = nullptr;
    std::unique_ptr<IndexBuffer> m_IndexBuffer = nullptr;
};
