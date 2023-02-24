#pragma once

#include "Vulkan/Types.h"

class Engine
{
public:
    void Init();
    void Destroy();

    void Draw();
    void Run();

private:
    void InitVulkan();
    void InitSwapchain();
    void InitDefaultRenderPass();
    void InitFramebuffers();
    void InitCommands();
    void InitSyncStructs();
    
    struct GLFWwindow* m_Window = nullptr;
    VkExtent2D m_Extent{800, 600};

    bool m_bInitialized = false;
    int m_FrameNumber = 0;

    vk::Instance m_Instance;
    vk::DebugUtilsMessengerEXT m_DebugMessenger;
    vk::PhysicalDevice m_PhysicalDevice;
    vk::Device m_Device;

    vk::Semaphore m_PresentSemaphore, m_RenderSemaphore;
    vk::Fence m_RenderFence;

    vk::Queue m_GraphicsQueue;
    uint32_t m_GraphicsQueueFamily = 0;

    vk::CommandPool m_CommandPool;
    vk::CommandBuffer m_MainCommandBuffer;

    vk::RenderPass m_RenderPass;

    vk::SurfaceKHR m_Surface;
    vk::SwapchainKHR m_Swapchain;
    vk::Format m_SwapchainImageFormat = vk::Format::eUndefined;

    std::vector<vk::Framebuffer> m_Framebuffers;
    std::vector<vk::Image> m_SwapchainImages;
    std::vector<vk::ImageView> m_SwapchainImageViews;
};
