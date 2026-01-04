#pragma once
#include <vulkan/vulkan.hpp>

#include "Vulkan/DescriptorSetWriter.h"

class Material
{
public:
    Material(vk::DescriptorSetLayout layout);
    ~Material();
    
    void SetAlbedo(const std::string& path);
    const std::string& GetAlbedoPath() const { return m_AlbedoPath; }
    
    void Bind(vk::CommandBuffer commandBuffer);

private:
    std::shared_ptr<Texture> m_Albedo;
    
    std::string m_AlbedoPath;
    vk::DescriptorSet m_DescriptorSet;
    DescriptorSetWriter m_Writer;
};
