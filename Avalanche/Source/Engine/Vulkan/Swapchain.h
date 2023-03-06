#pragma once

#include <vulkan/vulkan.hpp>

class Swapchain final
{
public:
    Swapchain(uint32_t width, uint32_t height);
    ~Swapchain();

private:
    void QueryInfo(uint32_t width, uint32_t height);
    void GetImages();
    void CreateImageViews();

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
    SwapchainInfo m_SwapchainInfo;
};
