#pragma once

#include <vulkan/vulkan.hpp>

class Swapchain
{
public:
    Swapchain(vk::PhysicalDevice physicalDevice, vk::Device device, vk::SurfaceKHR surface, uint32_t width, uint32_t height);

    void CreateFramebuffers(vk::Device device);
    void CreateRenderpass(vk::Device device);

    vk::RenderPass GetRenderPass() const { return m_RenderPass; }
    vk::SwapchainKHR GetSwapchain() const { return m_Swapchain; }
    vk::Extent2D GetExtent() const { return m_Extent; }
    vk::Semaphore GetSemaphore() const { return m_PresentSemaphore; }
    uint32_t GetSwapchainImageIndex() const { return m_SwapchainImageIndex; }

    void Begin(vk::Device device, vk::ClearValue clearValue, vk::CommandBuffer commandBuffer);
    
    operator vk::SwapchainKHR() const { return m_Swapchain; }
    
private:
    uint32_t m_SwapchainImageIndex = 0;
    
    vk::Extent2D m_Extent;
    
    vk::Semaphore m_PresentSemaphore;
    
    vk::RenderPass m_RenderPass;
    
    vk::SwapchainKHR m_Swapchain;
    vk::Format m_SwapchainImageFormat = vk::Format::eUndefined;

    std::vector<vk::Framebuffer> m_Framebuffers;
    std::vector<vk::Image> m_SwapchainImages;
    std::vector<vk::ImageView> m_SwapchainImageViews;
};
