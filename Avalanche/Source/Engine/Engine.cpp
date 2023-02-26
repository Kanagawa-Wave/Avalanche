#include "Engine.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <VkBootstrap.h>

#include "Vulkan/Types.h"
#include "Vulkan/Initializer.h"

#include "Log/Log.h"

#ifdef _DEBUG
constexpr bool bEnableValidation = true;
#else
constexpr bool bEnableValidation = false;
#endif

void Engine::Init()
{
    Log::Init();
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    m_Window = glfwCreateWindow((int)m_Extent.width, (int)m_Extent.height, "Vulkan", nullptr, nullptr);

    InitVulkan();
    InitSwapchain();
    InitDefaultRenderPass();
    InitFramebuffers();
    InitCommands();
    InitSyncStructs();
    InitPipelines();

    m_bInitialized = true;
}

void Engine::Destroy()
{
    if (m_bInitialized)
    {
        m_Device.waitIdle();

        m_Device.destroyCommandPool(m_CommandPool);
        m_Device.destroyFence(m_RenderFence);
        m_Device.destroySemaphore(m_RenderSemaphore);
        m_Device.destroySemaphore(m_PresentSemaphore);
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
    CHECK(m_Device.waitForFences(m_RenderFence, true, 1000000000));
    m_Device.resetFences(m_RenderFence);

    m_MainCommandBuffer.reset();

    uint32_t swapchainImageIndex =
        m_Device.acquireNextImageKHR(m_Swapchain, 1000000000, m_PresentSemaphore, nullptr).value;

    const vk::CommandBufferBeginInfo commandBufferInfo =
        Initializer::CommandBufferBegin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    m_MainCommandBuffer.begin(commandBufferInfo);

    vk::ClearValue clearValue;
    const float flash = abs(sin((float)m_FrameNumber / 120.f));
    clearValue.setColor({0.0f, 0.0f, flash, 1.0f});

    vk::RenderPassBeginInfo renderPassInfo = Initializer::RenderpassBegin(
        m_RenderPass, m_Extent, m_Framebuffers[swapchainImageIndex]);
    renderPassInfo.setClearValues(clearValue);

    m_MainCommandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

    /////////////////////////////Rendering/////////////////////////////

    m_MainCommandBuffer.endRenderPass();
    m_MainCommandBuffer.end();

    // Submit queue
    vk::SubmitInfo submitInfo = Initializer::SubmitInfo(m_MainCommandBuffer);
    vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    submitInfo.setWaitDstStageMask(waitStage)
              .setWaitSemaphores(m_PresentSemaphore)
              .setSignalSemaphores(m_RenderSemaphore);

    m_GraphicsQueue.submit(submitInfo, m_RenderFence);

    vk::PresentInfoKHR presentInfo = Initializer::PresentInfo();
    presentInfo.setSwapchains(m_Swapchain);
    presentInfo.setWaitSemaphores(m_RenderSemaphore);
    presentInfo.setImageIndices(swapchainImageIndex);

    CHECK(m_GraphicsQueue.presentKHR(presentInfo));

    m_FrameNumber++;
}

void Engine::Run()
{
    while (!glfwWindowShouldClose(m_Window))
    {
        glfwPollEvents();
        Draw();
    }
}

void Engine::InitVulkan()
{
    vkb::InstanceBuilder builder;
    auto instRet = builder.set_app_name("Vulkan App")
                          .request_validation_layers(bEnableValidation)
                          .use_default_debug_messenger()
                          .require_api_version(1, 3, 0)
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
               .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);

    vk::SubpassDependency dependency;
    dependency.setSrcSubpass(VK_SUBPASS_EXTERNAL)
              .setDstSubpass(0)
              .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
              .setSrcAccessMask(vk::AccessFlagBits::eNone)
              .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
              .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);

    vk::RenderPassCreateInfo renderPassInfo;
    renderPassInfo.setAttachments(attachmentInfo)
                  .setSubpasses(subpassInfo)
                  .setDependencies(dependency);

    m_RenderPass = m_Device.createRenderPass(renderPassInfo);
}

void Engine::InitFramebuffers()
{
    vk::FramebufferCreateInfo framebufferInfo = Initializer::FramebufferCreate(m_RenderPass, m_Extent);

    m_Framebuffers.resize(m_SwapchainImages.size());

    for (uint32_t i = 0; i < m_SwapchainImages.size(); i++)
    {
        framebufferInfo.setAttachments(m_SwapchainImageViews[i]);
        m_Framebuffers[i] = m_Device.createFramebuffer(framebufferInfo);
    }
}

void Engine::InitCommands()
{
    const vk::CommandPoolCreateInfo commandPoolInfo = Initializer::CommandPoolCreate(
        m_GraphicsQueueFamily, vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
    m_CommandPool = m_Device.createCommandPool(commandPoolInfo);

    const vk::CommandBufferAllocateInfo commandBufferInfo = Initializer::CommandBufferAllocate(m_CommandPool);
    m_MainCommandBuffer = m_Device.allocateCommandBuffers(commandBufferInfo).front();
}

void Engine::InitSyncStructs()
{
    const vk::FenceCreateInfo fenceInfo = Initializer::FenceCreate(vk::FenceCreateFlagBits::eSignaled);
    m_RenderFence = m_Device.createFence(fenceInfo);

    const vk::SemaphoreCreateInfo semaphoreInfo = Initializer::SemaphoreCreate();
    m_RenderSemaphore = m_Device.createSemaphore(semaphoreInfo);
    m_PresentSemaphore = m_Device.createSemaphore(semaphoreInfo);
}

void Engine::InitPipelines()
{
    vk::ShaderModule triangleVert, triangleFrag;
    LoadShaderModule("Shaders/Triangle", ShaderType::eVertex, triangleVert);
    LoadShaderModule("Shaders/Triangle", ShaderType::eFragment, triangleFrag);
}

void Engine::LoadShaderModule(std::string path, ShaderType type, vk::ShaderModule& shaderModule) const
{
    switch (type)
    {
    case ShaderType::eVertex:
        path.append(".vert.spv");
        break;
    case ShaderType::eFragment:
        path.append(".frag.spv");
        break;
    case ShaderType::eCompute:
        path.append(".comp.spv");
        break;
    }
    std::ifstream file(path, std::ios::ate | std::ios::binary);

    ASSERT(file.is_open(), "Failed to open shader file at {}", path)

    const long long fileSize = file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    vk::ShaderModuleCreateInfo shaderInfo;
    shaderInfo.setPCode(reinterpret_cast<const uint32_t*>(buffer.data()))
              .setCodeSize(buffer.size());

    shaderModule = m_Device.createShaderModule(shaderInfo);
}
