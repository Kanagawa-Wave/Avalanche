#include "Billboard.h"

#include <glm/glm.hpp>

#include "Renderer/Context.h"

Billboard::Billboard(const std::string& texturePath)
    : m_TexturePath(texturePath)
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
    
    ASSERT(context.GetBillboardDescriptorSetLayout(), "Invalid MaterialDescriptorSetLayout, please set layout in context")
    m_DescriptorSet = std::make_unique<DescriptorSet>(context.GetBillboardDescriptorSetLayout());
    
    SetTexture(texturePath);
}

Billboard::~Billboard()
= default;

void Billboard::Bind(vk::CommandBuffer commandBuffer) const
{
    const auto& context = Context::Instance();
    
    m_VertexBuffer->Bind(commandBuffer);
    m_IndexBuffer->Bind(commandBuffer);
    m_DescriptorSet->Bind(commandBuffer, context.GetCurrentPipelineLayout(), Context::PER_MATERIAL_SET);
}

void Billboard::Draw(vk::CommandBuffer commandBuffer) const
{
    commandBuffer.drawIndexed(m_IndexBuffer->GetCount(), 1, 0, 0, 0);
}

void Billboard::SetTexture(const std::string& texturePath)
{
    const Context& context = Context::Instance();
    m_Texture.reset(new Texture(texturePath, ETextureFormat::Linear));
    m_DescriptorSet->SetTexture(0, m_Texture.get());
    context.GetDevice().waitIdle();
    m_DescriptorSet->WriteAndClear();
}
