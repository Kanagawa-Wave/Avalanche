#include "Renderer.h"

#include "Context.h"
#include "Engine/Log/Log.h"

#include <glm/glm.hpp>

Renderer::Renderer()
{
    AllocateCommandBuffer();
    CreateFence();
    CreateSemaphores();
}

Renderer::~Renderer()
{
    const auto& device = Context::Instance().GetDevice();
    device.waitIdle();
    device.destroySemaphore(m_RenderSemaphore);
    device.destroySemaphore(m_PresentSemaphore);
    device.destroyFence(m_Fence);
}

void Renderer::Render(Mesh* mesh)
{
    const auto& device = Context::Instance().GetDevice();
    const auto& pass = Context::Instance().GetPipeline();
    const auto& swapchain = Context::Instance().GetSwapchain();

    if (device.waitForFences(m_Fence, true, std::numeric_limits<std::uint64_t>::max()) != vk::Result::eSuccess)
    {
        ASSERT(0, "Failed to wait for fence")
    }
    device.resetFences(m_Fence);

    const auto result = device.acquireNextImageKHR(swapchain.GetSwapchain(),
                                                   std::numeric_limits<uint64_t>::max(), m_PresentSemaphore);
    if (result.result != vk::Result::eSuccess)
    {
        ASSERT(0, "Failed to acquire next swapchain image")
    }

    uint32_t imageIndex = result.value;

    m_CommandBuffer.reset();

    vk::CommandBufferBeginInfo commandBufferBegin;
    commandBufferBegin.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    m_CommandBuffer.begin(commandBufferBegin);
    {
        m_CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pass.GetPipeline());
        m_CommandBuffer.bindVertexBuffers(0, mesh->GetVertexBuffer().GetBuffer(), {0});
        vk::RenderPassBeginInfo renderPassBegin;
        vk::Rect2D area;
        vk::ClearValue clearValue;
        area.setOffset({0, 0})
            .setExtent(swapchain.GetExtent());
        clearValue.setColor({0.1f, 0.1f, 0.1f, 1.0f});
        renderPassBegin.setRenderPass(pass.GetRenderPass())
                       .setRenderArea(area)
                       .setFramebuffer(swapchain.GetFramebuffer(imageIndex))
                       .setClearValues(clearValue);
        m_CommandBuffer.beginRenderPass(renderPassBegin, {});
        m_CommandBuffer.draw(3, 1, 0, 0);
        m_CommandBuffer.endRenderPass();
    }
    m_CommandBuffer.end();

    vk::SubmitInfo submit;
    vk::PipelineStageFlags flags = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    submit.setCommandBuffers(m_CommandBuffer)
          .setWaitSemaphores(m_PresentSemaphore)
          .setWaitDstStageMask(flags)
          .setSignalSemaphores(m_RenderSemaphore);
    Context::Instance().GetGraphicsQueue().submit(submit, m_Fence);


    vk::PresentInfoKHR present;
    present.setWaitSemaphores(m_RenderSemaphore)
           .setImageIndices(imageIndex)
           .setSwapchains(swapchain.GetSwapchain());
    if (Context::Instance().GetPresentQueue().presentKHR(present) != vk::Result::eSuccess)
    {
        ASSERT(0, "Failed to present image")
    }
}

void Renderer::AllocateCommandBuffer()
{
    m_CommandBuffer = Context::Instance().GetCommandManager().AllocateCommandBuffer();
}

void Renderer::CreateSemaphores()
{
    const auto& device = Context::Instance().GetDevice();
    const vk::SemaphoreCreateInfo semaphoreInfo;
    m_RenderSemaphore = device.createSemaphore(semaphoreInfo);
    m_PresentSemaphore = device.createSemaphore(semaphoreInfo);
}

void Renderer::CreateFence()
{
    vk::FenceCreateInfo fenceInfo;
    fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
    m_Fence = Context::Instance().GetDevice().createFence(fenceInfo);
}
