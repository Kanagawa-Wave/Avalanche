#include "Engine.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <VkBootstrap.h>

#include "Vulkan/Types.h"
#include "Vulkan/Initializer.h"

#ifdef _DEBUG
constexpr bool bEnableValidation = true;
#else
constexpr bool bEnableValidation = false;
#endif

void Engine::Init()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    m_Window = glfwCreateWindow((int)m_Extent.width, (int)m_Extent.height, "Vulkan", nullptr, nullptr);

    InitVulkan();
    InitSwapchain();
    InitDefaultRenderPass();
    InitFramebuffers();
    InitCommands();
    InitSyncStructs();

    m_bInitialized = true;
}

void Engine::Destroy()
{
    if (m_bInitialized)
    {
        vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);
        for (const auto& m_SwapchainImageView : m_SwapchainImageViews)
            vkDestroyImageView(m_Device, m_SwapchainImageView, nullptr);

        m_Device.destroy();
        vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
        vkb::destroy_debug_utils_messenger(m_Instance, m_DebugMessenger);
        m_Instance.destroy();
        glfwDestroyWindow(m_Window);
    }
}

void Engine::Draw()
{
}

void Engine::Run()
{
    while (!glfwWindowShouldClose(m_Window))
    {
        glfwPollEvents();
    }
}

void Engine::InitVulkan()
{
    vkb::InstanceBuilder builder;
    auto instRet = builder.set_app_name("Vulkan App")
                          .request_validation_layers(bEnableValidation)
                          .use_default_debug_messenger()
                          .require_api_version(1, 2, 0)
                          .build();

    vkb::Instance instance = instRet.value();

    m_Instance = instance.instance;
    m_DebugMessenger = instance.debug_messenger;

    glfwCreateWindowSurface(m_Instance, m_Window, nullptr, (VkSurfaceKHR*)&m_Surface);

    vkb::PhysicalDeviceSelector selector(instance);
    vkb::PhysicalDevice physicalDevice = selector.set_minimum_version(1, 2)
                                                 .set_surface(m_Surface)
                                                 .select()
                                                 .value();

    vkb::DeviceBuilder deviceBuilder(physicalDevice);
    vkb::Device device = deviceBuilder.build().value();

    m_Device = device.device;
    m_PhysicalDevice = physicalDevice.physical_device;

    m_GraphicsQueue = device.get_queue(vkb::QueueType::graphics).value();
    m_GraphicsQueueFamily = device.get_queue_index(vkb::QueueType::graphics).value();
}

void Engine::InitSwapchain()
{
    vkb::SwapchainBuilder swapchainBuilder(m_PhysicalDevice, m_Device, m_Surface);
    vkb::Swapchain swapchain = swapchainBuilder.use_default_format_selection()
                                               .set_desired_present_mode(VK_PRESENT_MODE_IMMEDIATE_KHR)
                                               .set_desired_extent(m_Extent.width, m_Extent.height)
                                               .build()
                                               .value();

    m_Swapchain = swapchain.swapchain;
    m_SwapchainImages = swapchain.get_images().value();
    m_SwapchainImageViews = swapchain.get_image_views().value();
    m_SwapchainImageFormat = swapchain.image_format;
}

void Engine::InitDefaultRenderPass()
{
}

void Engine::InitFramebuffers()
{
}

void Engine::InitCommands()
{
    vk::CommandPoolCreateInfo commandPoolInfo;
    commandPoolInfo.setQueueFamilyIndex(m_GraphicsQueueFamily)
                   .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

    m_CommandPool = m_Device.createCommandPool(commandPoolInfo);

    vk::CommandBufferAllocateInfo commandBufferInfo;
    commandBufferInfo.setCommandPool(m_CommandPool)
                     .setCommandBufferCount(1)
                     .setLevel(vk::CommandBufferLevel::ePrimary);

    m_MainCommandBuffer = m_Device.allocateCommandBuffers(commandBufferInfo).front();
}

void Engine::InitSyncStructs()
{
}
