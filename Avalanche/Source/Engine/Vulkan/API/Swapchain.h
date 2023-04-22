#pragma once

#include <vulkan/vulkan.hpp>

#include "Image.h"

class Swapchain final
{
public:
    Swapchain(uint32_t width, uint32_t height);
    ~Swapchain();
    
    const vk::Format& GetFormat() const { return m_SwapchainInfo.Format.format; }
    const vk::Extent2D& GetExtent() const { return m_SwapchainInfo.Extent; }
    const vk::Framebuffer& GetFramebuffer(uint32_t index) const { return m_Framebuffers[index]; }
    const vk::SwapchainKHR& GetSwapchain() const { return m_Swapchain; }
    const uint32_t& GetImageCount() const { return m_SwapchainInfo.Count; }
    void CreateFramebuffers(uint32_t width, uint32_t height, vk::RenderPass renderPass);

private:
    void QueryInfo(uint32_t width, uint32_t height);
    void GetImages();
    void CreateImageViews();
    void CreateDepthBuffer(uint32_t width, uint32_t height);

private:
    struct SwapchainInfo
    {
        vk::Extent2D Extent;
        uint32_t Count = 0;
        vk::SurfaceFormatKHR Format;
        vk::SurfaceTransformFlagsKHR Transform;
        vk::PresentModeKHR PresentMode = vk::PresentModeKHR::eFifo;
    };
    vk::SwapchainKHR m_Swapchain;
    std::vector<vk::Image> m_Images;
    std::vector<vk::ImageView> m_ImageViews;
    std::vector<vk::Framebuffer> m_Framebuffers;
    
    std::unique_ptr<Image> m_DepthStencil;
    SwapchainInfo m_SwapchainInfo;
};
