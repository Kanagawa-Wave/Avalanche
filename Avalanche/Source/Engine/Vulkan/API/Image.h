#pragma once

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

class Image
{
public:
    Image(vk::Format format, vk::Extent2D extent, vk::ImageUsageFlags usage);
    ~Image();

    vk::Image GetImage() const { return m_Image; }
    vk::ImageView GetView() const { return m_ImageView; }
    
private:
    vk::Image m_Image;
    vk::ImageView m_ImageView;
    VmaAllocation m_Allocation = VK_NULL_HANDLE;
};
