#pragma once
#include "Renderer/Texture.h"
#include "Renderer/Vulkan/Buffers.h"
#include "Renderer/Vulkan/DescriptorSet.h"

class Billboard
{
public:
    Billboard(const std::string& texturePath);
    ~Billboard();

    void Bind(vk::CommandBuffer commandBuffer) const;
    void Draw(vk::CommandBuffer commandBuffer) const;

    void SetTexture(const std::string& texturePath);
    Texture* GetTexture() const { return m_Texture.get(); }

    std::string GetTexturePath() const { return m_TexturePath; }

    static BufferLayout Layout() { return  {{ShaderDataType::Float2, "position"}}; }

private:
    std::string m_TexturePath;
    std::unique_ptr<DescriptorSet> m_DescriptorSet;
    std::unique_ptr<IndexBuffer> m_IndexBuffer;
    std::unique_ptr<VertexBuffer> m_VertexBuffer;
    std::unique_ptr<Texture> m_Texture;
};
