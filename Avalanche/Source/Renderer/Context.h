#pragma once

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>
#include <optional>

#include "CommandManager.h"
#include "DescriptorArena.h"

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
    DescriptorArena* GetDescriptorArena() const { return m_DescriptorArena.get(); }
    VmaAllocator GetAllocator() const { return m_Allocator; }
    CommandManager* GetCommandManager() const { return m_CommandManager.get(); }
    uint32_t GetGraphicsQueueFamilyIndex() const { return m_QueueFamilyIndices.GraphicsQueue.value(); }
    uint32_t GetPresentQueueFamilyIndex() const { return m_QueueFamilyIndices.PresentQueue.value(); }
    vk::PipelineLayout GetCurrentPipelineLayout() const { return m_CurrentPipelineLayout; }
    vk::DescriptorSetLayout GetMaterialDescriptorSetLayout() const { return m_MaterialDescriptorSetLayout; }
    vk::DescriptorSetLayout GetBillboardDescriptorSetLayout() const { return m_BillboardDescriptorSetLayout; }
    
    void SetCurrentPipelineLayout(vk::PipelineLayout pipelineLayout) {m_CurrentPipelineLayout = pipelineLayout; }
    void SetMaterialDescriptorSetLayout(vk::DescriptorSetLayout descriptorSetLayout) {m_MaterialDescriptorSetLayout = descriptorSetLayout; }
    void SetBillboardDescriptorSetLayout(vk::DescriptorSetLayout descriptorSetLayout) {m_BillboardDescriptorSetLayout = descriptorSetLayout; }
        
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
    void CreateDescriptorArena();
    void InitAllocator();
    
public:
    static constexpr int GLOBAL_SET = 0, PER_MATERIAL_SET = 1;

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

    vk::DescriptorSetLayout m_MaterialDescriptorSetLayout, m_BillboardDescriptorSetLayout;
    vk::Instance m_Instance;
    vk::PhysicalDevice m_PhysicalDevice;
    vk::Device m_Device;
    vk::Queue m_GraphicsQueue, m_PresentQueue;
    vk::SurfaceKHR m_Surface;
    // vk::DescriptorPool m_DescriptorPool;
    VmaAllocator m_Allocator = VK_NULL_HANDLE;
    
    std::unique_ptr<CommandManager> m_CommandManager;
    std::unique_ptr<DescriptorArena> m_DescriptorArena;
    
    vk::PipelineLayout m_CurrentPipelineLayout;
    
    QueueFamilyIndices m_QueueFamilyIndices;
};





