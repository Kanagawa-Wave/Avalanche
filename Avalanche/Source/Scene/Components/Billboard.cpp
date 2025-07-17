#include "Billboard.h"

#include <glm/glm.hpp>

#include "Renderer/Context.h"

Billboard::Billboard(float radius, const std::string& texturePath)
    : m_Radius(radius)
{
    const Context& context = Context::Instance();
    
    glm::vec2 vertices[6] = {
        {-1.0, -1.0},
        {-1.0, 1.0},
        {1.0, -1.0},
        {1.0, -1.0},
        {-1.0, 1.0},
        {1.0, 1.0}
    };
    uint32_t indices[6] = {0, 1, 2, 3, 4, 5};
    m_VertexBuffer = std::make_unique<VertexBuffer>(vertices, sizeof(glm::vec2) * 6);
    m_VertexBuffer->SetLayout(Layout());
    m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 6);
    m_DescriptorSet = context.GetDescriptorSetBuilder()->CreateDescriptorSet(EDescriptorSetLayoutType::PerModelSet);
    SetTexture(texturePath);
}

Billboard::~Billboard()
= default;

void Billboard::Bind(vk::CommandBuffer commandBuffer, const vk::PipelineLayout& layout) const
{
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, layout, 1, {m_DescriptorSet->GetDescriptorSet()}, nullptr);
    m_VertexBuffer->Bind(commandBuffer);
    m_IndexBuffer->Bind(commandBuffer);
}

void Billboard::Draw(vk::CommandBuffer commandBuffer) const
{
    commandBuffer.drawIndexed(m_IndexBuffer->GetCount(), 1, 0, 0, 0);
}

void Billboard::SetRadius(float radius)
{
    
}

void Billboard::SetTexture(const std::string& texturePath)
{
    m_Texture.reset(new Texture(texturePath, vk::Format::eR8G8B8A8Unorm));
    m_DescriptorSet->UpdateDescriptor(m_Texture.get(), 0);
}
