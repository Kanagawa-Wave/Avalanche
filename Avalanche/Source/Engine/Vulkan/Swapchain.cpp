#include "Swapchain.h"

#include "Initializer.h"
#include <VkBootstrap.h>

#include "Engine/Log/Log.h"

Swapchain::Swapchain(vk::PhysicalDevice physicalDevice, vk::Device device, vk::SurfaceKHR surface, uint32_t width,
                     uint32_t height)
    : m_Extent(width, height)
{
    // Swapchain
    vkb::SwapchainBuilder swapchainBuilder(physicalDevice, device, surface);
    vkb::Swapchain swapchain = swapchainBuilder.use_default_format_selection()
                                               .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
                                               .set_desired_extent(width, height)
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

    // Sync
    const vk::SemaphoreCreateInfo semaphoreInfo = Initializer::Semaphore();
    m_PresentSemaphore = device.createSemaphore(semaphoreInfo);
}

void Swapchain::CreateFramebuffers(vk::Device device)
{
    // Framebuffers
    vk::FramebufferCreateInfo framebufferInfo = Initializer::Framebuffer(m_RenderPass, m_Extent);

    m_Framebuffers.resize(m_SwapchainImages.size());

    for (uint32_t i = 0; i < m_SwapchainImages.size(); i++)
    {
        framebufferInfo.setAttachments(m_SwapchainImageViews[i]);
        m_Framebuffers[i] = device.createFramebuffer(framebufferInfo);
    }
}

void Swapchain::CreateRenderpass(vk::Device device)
{
    // Renderpass
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

    m_RenderPass = device.createRenderPass(renderPassInfo);
}

void Swapchain::Begin(vk::Device device, vk::ClearValue clearValue, vk::CommandBuffer commandBuffer)
{
    m_SwapchainImageIndex = device.acquireNextImageKHR(m_Swapchain, 1000000000, m_PresentSemaphore, nullptr).value;

    vk::RenderPassBeginInfo renderPassInfo = Initializer::RenderpassBegin(
        m_RenderPass, m_Extent, m_Framebuffers[m_SwapchainImageIndex]);
    renderPassInfo.setClearValues(clearValue);

    commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
}
