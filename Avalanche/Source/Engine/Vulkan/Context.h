#pragma once

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>
#include <optional>

#include "Pipeline.h"
#include "Swapchain.h"

struct GLFWwindow;

class Context final
{
public:
    static void Init(GLFWwindow* window);
    void Destroy();
    static Context& GetInstance();

    ~Context();

public:
    void CreateSwapchain(uint32_t width, uint32_t height);
    void CreatePipeline();
    
    vk::SurfaceKHR GetSurface() const { return m_Surface; }
    vk::PhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }
    vk::Device GetDevice() const { return m_Device; }
    Swapchain* GetSwapchain() { return m_Swapchain.get(); }
    Pipeline* GetPipeline() { return m_Pipeline.get(); }
    uint32_t GetGraphicsQueueFamilyIndex() const { return m_QueueFamilyIndices.GraphicsQueue.value(); }
    uint32_t GetPresentQueueFamilyIndex() const { return m_QueueFamilyIndices.PresentQueue.value(); }
        
private:
    Context(GLFWwindow* window);
    static std::unique_ptr<Context> s_Instance;

private:
    void CreateInstance(GLFWwindow* window);
    void ChoosePhysicalDevice();
    void CreateDevice();
    void QueryQueueFamilyIndices();
    void GetQueue();
    void CreateSurface(GLFWwindow* window);

private:
    struct QueueFamilyIndices final
    {
        std::optional<uint32_t> GraphicsQueue;
        std::optional<uint32_t> PresentQueue;

        operator bool() const
        {
            return GraphicsQueue.has_value() && PresentQueue.has_value();
        }
    };

    vk::Instance m_Instance;
    vk::PhysicalDevice m_PhysicalDevice;
    vk::Device m_Device;
    vk::Queue m_GraphicsQueue, m_PresentQueue;
    vk::SurfaceKHR m_Surface;
    std::unique_ptr<Swapchain> m_Swapchain;
    std::unique_ptr<Pipeline> m_Pipeline;
    
    QueueFamilyIndices m_QueueFamilyIndices;
};



