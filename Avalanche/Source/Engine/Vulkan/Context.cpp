#include "Context.h"

#include <GLFW/glfw3.h>
#include <VkBootstrap.h>

#include "Initializer.h"
#include "Engine/Log/Log.h"

/*! @brief Creates the rendering context for the specified window.
 *
 *  This function creates the rendering context for the specified windows.
 *
 *  It initializes the Vulkan instance, device, surface, command pools and command buffers.
 *
 *  @param[in] window The target GLFWwindow.
 */
Context::Context(GLFWwindow* window)
{
    // Instance & Device
    vkb::InstanceBuilder builder;
    auto instRet = builder.set_app_name("Avalanche")
                          .request_validation_layers(bEnableValidation)
                          .use_default_debug_messenger()
                          .require_api_version(1, 0, 0)
                          .build();

    vkb::Instance instance = instRet.value();

    m_Instance = instance.instance;
    m_DebugMessenger = instance.debug_messenger;

    glfwCreateWindowSurface(m_Instance, window, nullptr, (VkSurfaceKHR*)&m_Surface);

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

    // Command
    const vk::CommandPoolCreateInfo commandPoolInfo = Initializer::CommandPool(
    m_GraphicsQueueFamily, vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
    m_CommandPool = m_Device.createCommandPool(commandPoolInfo);

    const vk::CommandBufferAllocateInfo commandBufferInfo = Initializer::CommandBufferAllocate(m_CommandPool);
    m_MainCommandBuffer = m_Device.allocateCommandBuffers(commandBufferInfo).front();

    // Sync
    const vk::FenceCreateInfo fenceInfo = Initializer::Fence(vk::FenceCreateFlagBits::eSignaled);
    m_RenderFence = m_Device.createFence(fenceInfo);

    const vk::SemaphoreCreateInfo semaphoreInfo = Initializer::Semaphore();
    m_RenderSemaphore = m_Device.createSemaphore(semaphoreInfo);

    // VMA
    VmaAllocatorCreateInfo allocatorInfo{};
    allocatorInfo.physicalDevice = m_PhysicalDevice;
    allocatorInfo.device = m_Device;
    allocatorInfo.instance = m_Instance;

    vmaCreateAllocator(&allocatorInfo, &m_Allocator);
}

Context::~Context()
{
}

void Context::PollEvents()
{
    glfwPollEvents();
}

/*! @brief Creates a Vulkan swapchain in the specified dimensions.
 *
 *  This function creates a Vulkan swapchain in the specified dimensions.
 *
 *  It also creates the default Vulkan renderpass and framebuffer objects.
 *  
 *  If the swapchain of the context is already initialized, this function
 *  recreates the swapchain according to the input dimensions.
 *
 *  @param[in] width The width of the swapchain.
 *  @param[in] height The height of the swapchain.
 */
void Context::CreateSwapchain(uint32_t width, uint32_t height)
{
    m_Swapchain = std::make_unique<Swapchain>(m_PhysicalDevice, m_Device, m_Surface, width, height);
    m_Swapchain->CreateRenderpass(m_Device);
    m_Swapchain->CreateFramebuffers(m_Device);
}

void Context::CreatePipeline(const std::string& vsh, const std::string& fsh)
{
    m_Pipeline = std::make_unique<Pipeline>(m_Device, m_Swapchain->GetRenderPass(), m_Swapchain->GetExtent(), vsh, fsh);
}

void Context::CreatePipeline(const std::string& sh)
{
    m_Pipeline = std::make_unique<Pipeline>(m_Device, m_Swapchain->GetRenderPass(), m_Swapchain->GetExtent(), sh, sh);
}

void Context::Begin(vk::ClearValue clearValue) const
{
    CHECK(m_Device.waitForFences(m_RenderFence, true, 1000000000));
    m_Device.resetFences(m_RenderFence);

    m_MainCommandBuffer.reset();

    const vk::CommandBufferBeginInfo commandBufferInfo =
        Initializer::CommandBufferBegin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    m_MainCommandBuffer.begin(commandBufferInfo);

    m_Swapchain->Begin(m_Device, clearValue, m_MainCommandBuffer);

    m_MainCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_Pipeline->GetPipeline());
}

void Context::End()
{
    m_MainCommandBuffer.endRenderPass();
    m_MainCommandBuffer.end();
}

void Context::Submit()
{
    // Submit queue
    vk::Semaphore presentSemaphore = m_Swapchain->GetSemaphore();
    vk::SubmitInfo submitInfo = Initializer::Submit(m_MainCommandBuffer);
    vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    submitInfo.setWaitDstStageMask(waitStage)
              .setWaitSemaphores(presentSemaphore)
              .setSignalSemaphores(m_RenderSemaphore);

    m_GraphicsQueue.submit(submitInfo, m_RenderFence);

    vk::SwapchainKHR swapchain = m_Swapchain->GetSwapchain();
    uint32_t swapchainImageIndex = m_Swapchain->GetSwapchainImageIndex();
    vk::PresentInfoKHR presentInfo = Initializer::Present();
    presentInfo.setSwapchains(swapchain);
    presentInfo.setWaitSemaphores(m_RenderSemaphore);
    presentInfo.setImageIndices(swapchainImageIndex);

    CHECK(m_GraphicsQueue.presentKHR(presentInfo));
}

void Context::Draw(uint32_t vertexCount) const
{
    m_MainCommandBuffer.draw(vertexCount, 1, 0, 0);
}
