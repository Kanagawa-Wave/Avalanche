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
        m_Device.destroyCommandPool(m_CommandPool);
        m_Device.destroySwapchainKHR(m_Swapchain);
        m_Device.destroyRenderPass(m_RenderPass);
        for (uint32_t i = 0; i < m_Framebuffers.size(); i++)
        {
            m_Device.destroyFramebuffer(m_Framebuffers[i]);
            m_Device.destroyImageView(m_SwapchainImageViews[i]);
        }

        m_Device.destroy();
        m_Instance.destroySurfaceKHR(m_Surface);
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
    m_SwapchainImages.resize(swapchain.image_count);
    for (uint32_t i = 0; i < swapchain.image_count; i++)
        m_SwapchainImages[i] = swapchain.get_images().value()[i];

    m_SwapchainImageViews.resize(swapchain.image_count);
    for (uint32_t i = 0; i < swapchain.image_count; i++)
        m_SwapchainImageViews[i] = swapchain.get_image_views().value()[i];
    
    m_SwapchainImageFormat = (vk::Format)swapchain.image_format;
}

void Engine::InitDefaultRenderPass()
{
    vk::AttachmentDescription attachmentInfo;
    attachmentInfo.setFormat(m_SwapchainImageFormat)
                  .setSamples(vk::SampleCountFlagBits::e1)
                  .setLoadOp(vk::AttachmentLoadOp::eClear)
                  .setStoreOp(vk::AttachmentStoreOp::eStore)
                  .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
                  .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
                  .setInitialLayout(vk::ImageLayout::eUndefined)
                  .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

    vk::AttachmentReference attachmentRef;
    attachmentRef.setAttachment(0)
                 .setLayout(vk::ImageLayout::eColorAttachmentOptimal);

    vk::SubpassDescription subpassInfo;
    subpassInfo.setColorAttachments(attachmentRef)
               .setColorAttachmentCount(1)
               .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);

    vk::RenderPassCreateInfo renderPassInfo;
    renderPassInfo.setAttachments(attachmentInfo)
                  .setAttachmentCount(1)
                  .setSubpasses(subpassInfo)
                  .setSubpassCount(1);

    m_RenderPass = m_Device.createRenderPass(renderPassInfo);
}

void Engine::InitFramebuffers()
{
    vk::FramebufferCreateInfo framebufferInfo;
    framebufferInfo.setRenderPass(m_RenderPass)
                   .setAttachmentCount(1)
                   .setWidth(m_Extent.width)
                   .setHeight(m_Extent.height)
                   .setLayers(1);

    m_Framebuffers.resize(m_SwapchainImages.size());

    for (uint32_t i = 0; i < m_SwapchainImages.size(); i++)
    {
        framebufferInfo.setAttachments(m_SwapchainImageViews[i]);
        m_Framebuffers[i] = m_Device.createFramebuffer(framebufferInfo);
    }
    
}

void Engine::InitCommands()
{
    m_CommandPool = m_Device.createCommandPool(
        Initializer::CommandPool(m_GraphicsQueueFamily, vk::CommandPoolCreateFlagBits::eResetCommandBuffer));

    m_MainCommandBuffer = m_Device.allocateCommandBuffers(Initializer::CommandBuffer(m_CommandPool)).front();
}

void Engine::InitSyncStructs()
{
    vk::FenceCreateInfo fenceInfo;
    fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);

    m_RenderFence = m_Device.createFence(fenceInfo);

    vk::SemaphoreCreateInfo semaphoreInfo;
    m_RenderSemaphore = m_Device.createSemaphore(semaphoreInfo);
    m_PresentSemaphore = m_Device.createSemaphore(semaphoreInfo);
}
