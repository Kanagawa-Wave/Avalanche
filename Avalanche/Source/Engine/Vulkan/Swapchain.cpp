#include "Swapchain.h"

#include "Context.h"

Swapchain::Swapchain(uint32_t width, uint32_t height)
{
    QueryInfo(width, height);

    vk::SwapchainCreateInfoKHR swapchainInfo;
    swapchainInfo.setClipped(true)
                 .setImageArrayLayers(1)
                 .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
                 .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
                 .setSurface(Context::GetInstance().GetSurface())
                 .setImageExtent(m_SwapchainInfo.Extent)
                 .setImageColorSpace(m_SwapchainInfo.Format.colorSpace)
                 .setImageFormat(m_SwapchainInfo.Format.format)
                 .setMinImageCount(m_SwapchainInfo.Count)
                 .setPresentMode(m_SwapchainInfo.PresentMode);

    uint32_t graphicsFamilyIndex = Context::GetInstance().GetGraphicsQueueFamilyIndex();
    uint32_t presentFamilyIndex = Context::GetInstance().GetPresentQueueFamilyIndex();
    if (graphicsFamilyIndex == presentFamilyIndex)
    {
        swapchainInfo.setQueueFamilyIndices(graphicsFamilyIndex)
                     .setImageSharingMode(vk::SharingMode::eExclusive);
    }
    else
    {
        std::array indices = {graphicsFamilyIndex, presentFamilyIndex};
        swapchainInfo.setQueueFamilyIndices(indices)
                     .setImageSharingMode(vk::SharingMode::eConcurrent);
    }

    m_Swapchain = Context::GetInstance().GetDevice().createSwapchainKHR(swapchainInfo);

    GetImages();
    CreateImageViews();
}

Swapchain::~Swapchain()
{
    for (const auto view : m_ImageViews)
    {
        Context::GetInstance().GetDevice().destroyImageView(view);
    }
    Context::GetInstance().GetDevice().destroySwapchainKHR(m_Swapchain);
}

void Swapchain::QueryInfo(uint32_t width, uint32_t height)
{
    const auto device = Context::GetInstance().GetPhysicalDevice();
    const auto surface = Context::GetInstance().GetSurface();
    const auto formats = device.getSurfaceFormatsKHR(surface);
    m_SwapchainInfo.Format = formats[0];
    for (const auto format : formats)
    {
        if (format.format == vk::Format::eR8G8B8A8Srgb &&
            format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            m_SwapchainInfo.Format = format;
            break;
        }
    }

    const auto capabilities = device.getSurfaceCapabilitiesKHR(surface);
    m_SwapchainInfo.Count = capabilities.minImageCount;
    m_SwapchainInfo.Extent.width = std::clamp<uint32_t>(width, capabilities.minImageExtent.width,
                                                        capabilities.maxImageExtent.width);
    m_SwapchainInfo.Extent.height = std::clamp<uint32_t>(height, capabilities.minImageExtent.height,
                                                         capabilities.maxImageExtent.height);
    m_SwapchainInfo.Transform = capabilities.currentTransform;

    const auto presentModes = device.getSurfacePresentModesKHR(surface);
    for (const auto presentMode : presentModes)
    {
        if (presentMode == vk::PresentModeKHR::eMailbox)
        {
            m_SwapchainInfo.PresentMode = presentMode;
            break;
        }
    }
}

void Swapchain::GetImages()
{
    m_Images = Context::GetInstance().GetDevice().getSwapchainImagesKHR(m_Swapchain);
}

void Swapchain::CreateImageViews()
{
    m_ImageViews.resize(m_Images.size());

    for (uint32_t i = 0; i < m_ImageViews.size(); i++)
    {
        vk::ImageViewCreateInfo viewInfo;
        vk::ComponentMapping mapping;
        vk::ImageSubresourceRange range;
        range.setBaseMipLevel(0)
             .setLevelCount(1)
             .setBaseArrayLayer(0)
             .setLayerCount(1)
             .setAspectMask(vk::ImageAspectFlagBits::eColor);
        viewInfo.setImage(m_Images[i])
                .setViewType(vk::ImageViewType::e2D)
                .setComponents(mapping)
                .setFormat(m_SwapchainInfo.Format.format)
                .setSubresourceRange(range);
        m_ImageViews[i] = Context::GetInstance().GetDevice().createImageView(viewInfo);
    }
}
