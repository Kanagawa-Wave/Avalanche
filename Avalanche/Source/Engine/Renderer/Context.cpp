#include "Context.h"

#include "Engine/Core/Log.h"

#include <GLFW/glfw3.h>

#include "ImmediateContext.h"

std::unique_ptr<Context> Context::s_Instance = nullptr;

void Context::Init(GLFWwindow* window)
{
    s_Instance.reset(new Context(window));
    s_Instance->InitCommandManager();
    ImmediateContext::Init();
}

void Context::Destroy()
{
    m_Device.waitIdle();
    ImmediateContext::Shutdown();
    m_CommandManager.reset();
    m_Device.destroyDescriptorPool(m_DescriptorPool);
    m_Instance.destroySurfaceKHR(m_Surface);
    vmaDestroyAllocator(m_Allocator);
    m_Device.destroy();
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
    CreateDescriptorPool();
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
    deviceInfo.setQueueCreateInfos(queueInfos)
              .setPEnabledExtensionNames(extensions);

    m_Device = m_PhysicalDevice.createDevice(deviceInfo);
}

void Context::QueryQueueFamilyIndices()
{
    const auto properties = m_PhysicalDevice.getQueueFamilyProperties();
    for (uint32_t i = 0; i < properties.size(); i++)
    {
        if (properties[i].queueFlags | vk::QueueFlagBits::eGraphics)
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
}

void Context::CreateDescriptorPool()
{
    vk::DescriptorPoolCreateInfo poolInfo;
    std::vector<vk::DescriptorPoolSize> poolSizes = {
        {vk::DescriptorType::eUniformBuffer, 1000},
        {vk::DescriptorType::eCombinedImageSampler, 1000}
    };
    
    poolInfo.setPoolSizes(poolSizes)
            .setMaxSets(1000);

    m_DescriptorPool = m_Device.createDescriptorPool(poolInfo);
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
