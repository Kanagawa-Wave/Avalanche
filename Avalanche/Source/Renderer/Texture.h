#pragma once

#include "Vulkan/Image.h"

class Texture
{
public:
    Texture() = default;
    Texture(vk::Format format, vk::Extent2D extent, vk::ImageUsageFlags usage);
    Texture(const std::string& path, vk::Format format);
    ~Texture();

    void RegisterForImGui();
    
    operator Image*() const
    {
        return m_Image.get();
    }
    vk::Sampler GetSampler() const { return m_Sampler; }
    vk::ImageView GetView() const { return m_Image->GetView(); }
    void* GetTextureID() const { return m_DescriptorSet; }
    
private:
    int m_Width = 0, m_Height = 0, m_Channels = 0;
    std::unique_ptr<Image> m_Image;
    vk::Sampler m_Sampler;

    // ImGui Only
    vk::DescriptorSet m_DescriptorSet;
};
