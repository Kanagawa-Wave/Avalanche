#pragma once

#include <glm/glm.hpp>

#include "Renderer/Texture.h"
#include "Renderer/Vulkan/Buffers.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Renderer/Vulkan/DescriptorSet.h"

struct ModelVertex
{
    glm::vec3 position{};
    glm::vec3 color{};
    glm::vec3 normal{};
    glm::vec2 uv{};

    bool operator ==(const ModelVertex& other) const
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

    void Bind(vk::CommandBuffer commandBuffer, const vk::PipelineLayout& layout) const;
    void Draw(vk::CommandBuffer commandBuffer) const;

    void SetTexture(const std::string& path);
    
    Texture* GetTexture() const { return m_Texture.get(); }
    VertexBuffer* GetVertexBuffer() const { return m_VertexBuffer.get(); }
    IndexBuffer* GetIndexBuffer() const { return m_IndexBuffer.get(); }

    const std::string& GetMeshPath() const { return m_MeshPath; }
    const std::string& GetTexturePath() const { return m_TexturePath; }

private:
    void LoadObjFromFile(const std::string& path);
    void LoadMeshFromFile(const std::string& path);

    std::string m_MeshPath;
    std::string m_TexturePath;

    std::unique_ptr<Texture> m_Texture;
    std::unique_ptr<DescriptorSet> m_DescriptorSet;
    
    std::vector<ModelVertex> m_Vertices{};
    std::vector<uint32_t> m_Indices{};
    
    std::unique_ptr<VertexBuffer> m_VertexBuffer = nullptr;
    std::unique_ptr<IndexBuffer> m_IndexBuffer = nullptr;
};
