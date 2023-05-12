#pragma once

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>
#include <optional>

#include "API/Pipeline.h"
#include "API/Swapchain.h"
#include "CommandManager.h"

struct GLFWwindow;

class Context final
{
public:
    static void Init(GLFWwindow* window);
    void Destroy();
    static Context& Instance();

    ~Context();

public:
    void InitCommandManager();

    vk::Instance GetInstance() const { return m_Instance; }
    vk::SurfaceKHR GetSurface() const { return m_Surface; }
    vk::PhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }
    vk::Device GetDevice() const { return m_Device; }
    vk::Queue GetGraphicsQueue() const { return m_GraphicsQueue; }
    vk::Queue GetPresentQueue() const { return m_PresentQueue; }
    vk::DescriptorPool GetDescriptorPool() const { return m_DescriptorPool; }
    VmaAllocator GetAllocator() const { return m_Allocator; }
    CommandManager* GetCommandManager() const { return m_CommandManager.get(); }
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
    void CreateDescriptorPool();
    void InitAllocator();

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
    vk::DescriptorPool m_DescriptorPool;
    VmaAllocator m_Allocator = VK_NULL_HANDLE;
    std::unique_ptr<CommandManager> m_CommandManager;
    
    QueueFamilyIndices m_QueueFamilyIndices;
};





