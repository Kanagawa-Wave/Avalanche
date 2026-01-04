#pragma once

#include <glm/glm.hpp>

#include "Renderer/Texture.h"
#include "Renderer/Vulkan/Buffers.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Renderer/Material.h"
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

    void Bind(vk::CommandBuffer commandBuffer) const;
    void Draw(vk::CommandBuffer commandBuffer) const;
    
    Material* GetMaterial() const { return m_Material.get(); }
    VertexBuffer* GetVertexBuffer() const { return m_VertexBuffer.get(); }
    IndexBuffer* GetIndexBuffer() const { return m_IndexBuffer.get(); }

    const std::string& GetMeshPath() const { return m_MeshPath; }
    
    static void SetDescriptorSetLayout(const vk::DescriptorSetLayout& layout) { s_DescriptorSetLayout = layout; }

private:
    void LoadObjFromFile(const std::string& path);
    void LoadMeshFromFile(const std::string& path);

    std::string m_MeshPath;
    std::unique_ptr<Material> m_Material;
    
    std::vector<ModelVertex> m_Vertices{};
    std::vector<uint32_t> m_Indices{};
    
    std::unique_ptr<VertexBuffer> m_VertexBuffer = nullptr;
    std::unique_ptr<IndexBuffer> m_IndexBuffer = nullptr;
    
    static vk::DescriptorSetLayout s_DescriptorSetLayout;
};
