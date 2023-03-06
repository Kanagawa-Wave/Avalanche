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
}

Swapchain::~Swapchain()
{
    Context::GetInstance().GetDevice().destroySwapchainKHR(m_Swapchain);
}

void Swapchain::QueryInfo(uint32_t width, uint32_t height)
{
    auto device = Context::GetInstance().GetPhysicalDevice();
    auto surface = Context::GetInstance().GetSurface();
    auto formats = device.getSurfaceFormatsKHR(surface);
    m_SwapchainInfo.Format = formats[0];
    for (auto format : formats)
    {
        if (format.format == vk::Format::eR8G8B8A8Srgb &&
            format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            m_SwapchainInfo.Format = format;
            break;
        }
    }

    auto capabilities = device.getSurfaceCapabilitiesKHR(surface);
    m_SwapchainInfo.Count = capabilities.minImageCount;
    m_SwapchainInfo.Extent.width = std::clamp<uint32_t>(width, capabilities.minImageExtent.width,
                                                        capabilities.maxImageExtent.width);
    m_SwapchainInfo.Extent.height = std::clamp<uint32_t>(height, capabilities.minImageExtent.height,
                                                         capabilities.maxImageExtent.height);
    m_SwapchainInfo.Transform = capabilities.currentTransform;

    auto presentModes = device.getSurfacePresentModesKHR(surface);
    for (auto presentMode : presentModes)
    {
        if (presentMode == vk::PresentModeKHR::eMailbox)
        {
            m_SwapchainInfo.PresentMode = presentMode;
            break;
        }
    }
}
