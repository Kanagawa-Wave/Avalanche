#include "Context.h"

#include "Core/Log.h"

#include <GLFW/glfw3.h>

#include "ImmediateContext.h"

std::unique_ptr<Context> Context::s_Instance = nullptr;

void Context::Init(GLFWwindow* window)
{
    s_Instance.reset(new Context(window));
    s_Instance->InitCommandManager();
    s_Instance->CreateDescriptorArena();
    ImmediateContext::Init();
}

void Context::Destroy()
{
    m_Device.waitIdle();
    ImmediateContext::Instance().Destroy();
    m_DescriptorArena.reset();
    m_CommandManager.reset();
    
    LOG_T("Destroying VkSurface {}", fmt::ptr((VkSurfaceKHR)m_Surface))
    m_Instance.destroySurfaceKHR(m_Surface);
    
    vmaDestroyAllocator(m_Allocator);

    LOG_T("Destroying VkDevice {}", fmt::ptr((VkDevice)m_Device))
    m_Device.destroy();

    LOG_T("Destroying VkInstance {}", fmt::ptr((VkInstance)m_Instance))
    m_Instance.destroy();
    
    s_Instance.reset();
}

Context& Context::Instance()
{
    return *s_Instance;
}

Context::Context(GLFWwindow* window)
{
    CreateInstance(window);
    ChoosePhysicalDevice();
    CreateSurface(window);
    QueryQueueFamilyIndices();
    CreateDevice();
    GetQueue();
    InitAllocator();
}

void Context::CreateInstance(GLFWwindow* window)
{
    #ifdef _DEBUG
        std::vector<const char*> layers = {"VK_LAYER_KHRONOS_validation"};
    #else
        std::vector<const char*> layers;
    #endif
    
    uint32_t count;
    const char** extensions = glfwGetRequiredInstanceExtensions(&count);

    vk::InstanceCreateInfo instanceInfo;
    vk::ApplicationInfo applicationInfo;
    applicationInfo.setApiVersion(VK_API_VERSION_1_3);

    instanceInfo.setPApplicationInfo(&applicationInfo)
                .setPpEnabledExtensionNames(extensions)
                .setEnabledExtensionCount(count)
                .setPEnabledLayerNames(layers);

    m_Instance = vk::createInstance(instanceInfo);
    LOG_T("VkInstance {0} created successfully", fmt::ptr((VkInstance)m_Instance))
}

void Context::ChoosePhysicalDevice()
{
    const auto devices = m_Instance.enumeratePhysicalDevices();

    auto physicalDevice = devices.front();
    for (auto device : devices)
    {
        if (device.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
            physicalDevice = device;
    }

    LOG_T("Physical Device: [{0}] {1} ({2})", vk::to_string(physicalDevice.getProperties().deviceType),
          physicalDevice.getProperties().deviceName, physicalDevice.getProperties().driverVersion)

    m_PhysicalDevice = physicalDevice;
}

void Context::CreateDevice()
{
    std::array extensions = {"VK_KHR_swapchain"};
    vk::DeviceCreateInfo deviceInfo;
    std::vector<vk::DeviceQueueCreateInfo> queueInfos;

    float priority = 1.f;

    if (m_QueueFamilyIndices.PresentQueue.value() == m_QueueFamilyIndices.GraphicsQueue.value())
    {
        vk::DeviceQueueCreateInfo queueInfo;
        queueInfo.setQueuePriorities(priority)
                 .setQueueFamilyIndex(m_QueueFamilyIndices.GraphicsQueue.value());
        queueInfos.push_back(std::move(queueInfo));
    }
    else
    {
        vk::DeviceQueueCreateInfo queueInfo;
        queueInfo.setQueuePriorities(priority)
                 .setQueueFamilyIndex(m_QueueFamilyIndices.GraphicsQueue.value());
        queueInfos.push_back(queueInfo);
        queueInfo.setQueuePriorities(priority)
                 .setQueueFamilyIndex(m_QueueFamilyIndices.PresentQueue.value());
        queueInfos.push_back(queueInfo);
    }

    #ifdef _DEBUG
        std::vector<const char*> layers = {"VK_LAYER_KHRONOS_validation"};
    #else
        std::vector<const char*> layers;
    #endif
    
    deviceInfo.setQueueCreateInfos(queueInfos)
              .setPEnabledExtensionNames(extensions)
              .setPEnabledLayerNames(layers);
    m_Device = m_PhysicalDevice.createDevice(deviceInfo);
    LOG_T("VkDevice {0} created successfully", fmt::ptr((VkDevice)m_Device))
}

void Context::QueryQueueFamilyIndices()
{
    const auto properties = m_PhysicalDevice.getQueueFamilyProperties();
    for (uint32_t i = 0; i < properties.size(); i++)
    {
        if (properties[i].queueFlags & vk::QueueFlagBits::eGraphics)
        {
            m_QueueFamilyIndices.GraphicsQueue = i;
        }
        if (m_PhysicalDevice.getSurfaceSupportKHR(i, m_Surface))
        {
            m_QueueFamilyIndices.PresentQueue = i;
        }
        if (m_QueueFamilyIndices)
        {
            break;
        }
    }
}

void Context::GetQueue()
{
    m_GraphicsQueue = m_Device.getQueue(m_QueueFamilyIndices.GraphicsQueue.value(), 0);
    m_PresentQueue = m_Device.getQueue(m_QueueFamilyIndices.PresentQueue.value(), 0);
}

void Context::CreateSurface(GLFWwindow* window)
{
    glfwCreateWindowSurface(m_Instance, window, nullptr, (VkSurfaceKHR*)&m_Surface);
    LOG_T("VkSurface {0} created successfully", fmt::ptr((VkSurfaceKHR)m_Surface))
}

void Context::CreateDescriptorArena()
{
    m_DescriptorArena = std::make_unique<DescriptorArena>(1, 1000);
    
    // vk::DescriptorPoolCreateInfo poolInfo;
    // std::vector<vk::DescriptorPoolSize> poolSizes = {
    //     {vk::DescriptorType::eUniformBuffer, 1000},
    //     {vk::DescriptorType::eCombinedImageSampler, 1000}
    // };
    //
    // poolInfo.setPoolSizes(poolSizes)
    //         .setMaxSets(1000);
    //
    // m_DescriptorPool = m_Device.createDescriptorPool(poolInfo);
    // LOG_T("VkDescriptorPool {0} created successfully", fmt::ptr((VkDescriptorPool)m_DescriptorPool))
}

void Context::InitAllocator()
{
    VmaAllocatorCreateInfo vmaInfo{};
    vmaInfo.device = m_Device;
    vmaInfo.instance = m_Instance;
    vmaInfo.physicalDevice = m_PhysicalDevice;
    vmaCreateAllocator(&vmaInfo, &m_Allocator);
}

void Context::InitCommandManager()
{
    m_CommandManager = std::make_unique<CommandManager>();
}

Context::~Context()
{
}
