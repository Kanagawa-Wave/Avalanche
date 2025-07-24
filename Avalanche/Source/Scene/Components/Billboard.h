#pragma once
#include "Renderer/Texture.h"
#include "Renderer/Vulkan/Buffers.h"
#include "Renderer/Vulkan/DescriptorSet.h"

class Billboard
{
public:
    Billboard(float radius, const std::string& texturePath);
    ~Billboard();

    void Bind(vk::CommandBuffer commandBuffer, const vk::PipelineLayout& layout) const;
    void Draw(vk::CommandBuffer commandBuffer) const;

    void SetRadius(float radius);
    void SetTexture(const std::string& texturePath);
    float GetRadius() const { return m_Radius; }
    std::string GetTexturePath() const { return m_TexturePath; }

    static BufferLayout Layout() { return  {{ShaderDataType::Float2, "position"}}; }

private:
    float m_Radius;
    std::string m_TexturePath;
    std::unique_ptr<IndexBuffer> m_IndexBuffer;
    std::unique_ptr<VertexBuffer> m_VertexBuffer;
    std::unique_ptr<Texture> m_Texture;
    std::unique_ptr<DescriptorSet> m_DescriptorSet;
};
