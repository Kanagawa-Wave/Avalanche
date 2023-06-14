#include "Image.h"

#include "Renderer/Context.h"

Image::Image(vk::Format format, vk::Extent2D extent, vk::ImageUsageFlags usage)
{
    vk::Extent3D extent3D(extent, 1);

    vk::ImageCreateInfo imageInfo;
    imageInfo.setImageType(vk::ImageType::e2D)
             .setExtent(extent3D)
             .setFormat(format)
             .setSamples(vk::SampleCountFlagBits::e1)
             .setTiling(vk::ImageTiling::eOptimal)
             .setMipLevels(1)
             .setArrayLayers(1)
             .setUsage(usage);

    VmaAllocationCreateInfo allocInfo{};
    if (usage == vk::ImageUsageFlagBits::eDepthStencilAttachment)
    {
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocInfo.requiredFlags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
    }
    else
    {
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    }

    vmaCreateImage(Context::Instance().GetAllocator(), (VkImageCreateInfo*)&imageInfo, &allocInfo, (VkImage*)&m_Image,
                   &m_Allocation, nullptr);

    vk::ImageViewCreateInfo imageViewInfo;
    vk::ImageSubresourceRange subresourceRange;
    subresourceRange.setBaseMipLevel(0)
                    .setLevelCount(1)
                    .setBaseArrayLayer(0)
                    .setLayerCount(1);
    if (usage == vk::ImageUsageFlagBits::eDepthStencilAttachment)
        subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eDepth);
    else
        subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
    imageViewInfo.setViewType(vk::ImageViewType::e2D)
                 .setImage(m_Image)
                 .setFormat(format)
                 .setSubresourceRange(subresourceRange);

    m_ImageView = Context::Instance().GetDevice().createImageView(imageViewInfo);
}

Image::~Image()
{
    Context::Instance().GetDevice().destroyImageView(m_ImageView);
    vmaDestroyImage(Context::Instance().GetAllocator(), m_Image, m_Allocation);
}
