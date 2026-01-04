#include "Material.h"

#include "Context.h"

Material::Material(vk::DescriptorSetLayout layout)
{
    m_DescriptorSet = Context::Instance().GetDescriptorArena()->Allocate(layout);
}

Material::~Material()
= default;

void Material::SetAlbedo(const std::string& path)
{
    m_Albedo = std::make_shared<Texture>(path, ETextureFormat::Linear);
    m_AlbedoPath = path;
    m_Writer.SetTexture(0, m_Albedo.get());
    m_Writer.Write(m_DescriptorSet);
    m_Writer.Clear();
}

void Material::Bind(vk::CommandBuffer commandBuffer)
{
    commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics, 
        Context::Instance().GetCurrentPipelineLayout(), 
        Context::PER_MATERIAL_SET, 
        {m_DescriptorSet}, 
        nullptr);
}
