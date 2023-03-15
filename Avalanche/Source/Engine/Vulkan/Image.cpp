#include "Image.h"

#include "Context.h"

Image::Image(vk::Format format, vk::Extent2D extent, ImageUsage usage)
{
    switch (usage)
    {
    case ImageUsage::eDepthStencil:
        CreateImageForDepthStencil(format, extent);
        return;
    case ImageUsage::eTexture:
        return;
    }
}

Image::~Image()
{
    Context::Instance().GetDevice().destroyImageView(m_ImageView);
    vmaDestroyImage(Context::Instance().GetAllocator(), m_Image, m_Allocation);
}

void Image::CreateImageForDepthStencil(vk::Format format, vk::Extent2D extent)
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
             .setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment);

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    allocInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    vmaCreateImage(Context::Instance().GetAllocator(), (VkImageCreateInfo*)&imageInfo, &allocInfo, (VkImage*)&m_Image,
                   &m_Allocation, nullptr);

    vk::ImageViewCreateInfo imageViewInfo;
    vk::ImageSubresourceRange subresourceRange;
    subresourceRange.setBaseMipLevel(0)
                    .setLevelCount(1)
                    .setBaseArrayLayer(0)
                    .setLayerCount(1)
                    .setAspectMask(vk::ImageAspectFlagBits::eDepth);
    imageViewInfo.setViewType(vk::ImageViewType::e2D)
                 .setImage(m_Image)
                 .setFormat(format)
                 .setSubresourceRange(subresourceRange);

    m_ImageView = Context::Instance().GetDevice().createImageView(imageViewInfo);
}

void Image::CreateImageForTexture(vk::Format format, vk::Extent2D extent)
{
}
