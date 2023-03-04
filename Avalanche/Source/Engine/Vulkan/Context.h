#pragma once

#include <vulkan/vulkan.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Swapchain.h"
#include "Pipeline.h"

class Context
{
public:
    Context(GLFWwindow* window);
    ~Context();

    static void PollEvents();

    void CreateSwapchain(uint32_t width, uint32_t height);
    void CreatePipeline(const std::string& vsh, const std::string& fsh);
    void CreatePipeline(const std::string& sh);

    void Begin(vk::ClearValue clearValue) const;
    void End();
    void Submit();

    void Draw(uint32_t vertexCount) const;

private:
    std::unique_ptr<Swapchain> m_Swapchain;
    std::unique_ptr<Pipeline> m_Pipeline;

    vk::Instance m_Instance;
    vk::DebugUtilsMessengerEXT m_DebugMessenger;
    vk::PhysicalDevice m_PhysicalDevice;
    vk::SurfaceKHR m_Surface;
    vk::Device m_Device;

    vk::Semaphore m_RenderSemaphore;
    vk::Fence m_RenderFence;

    vk::Queue m_GraphicsQueue;
    uint32_t m_GraphicsQueueFamily = 0;

    vk::CommandPool m_CommandPool;
    vk::CommandBuffer m_MainCommandBuffer;

#ifdef _DEBUG
    const bool bEnableValidation = true;
#else
    const bool bEnableValidation = false;
#endif
};
