#pragma once

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

enum class ImageUsage
{
    eDepthStencil = 0,
    eTexture = 1
};

class Image final
{
public:
    Image(vk::Format format, vk::Extent2D extent, ImageUsage usage);
    ~Image();

    vk::Image GetImage() const { return m_Image; }
    vk::ImageView GetView() const { return m_ImageView; }
    
private:
    void CreateImageForDepthStencil(vk::Format format, vk::Extent2D extent);
    void CreateImageForTexture(vk::Format format, vk::Extent2D extent);
    
    vk::Image m_Image;
    vk::ImageView m_ImageView;
    VmaAllocation m_Allocation = VK_NULL_HANDLE;
};
