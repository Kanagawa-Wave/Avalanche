#include "Renderer.h"

#include "Context.h"
#include "Engine/Log/Log.h"

#include <glm/glm.hpp>

#include "glm/gtx/transform.hpp"

Renderer::Renderer(float aspect)
{
    AllocateCommandBuffer();
    CreateFence();
    CreateSemaphores();
    m_Camera = std::make_unique<Camera>(30.f, aspect, 0.001f, 10000.f);
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
    // TODO: remove
    glm::vec3 camPos = {0.f, 0.f, -2.f};

    glm::mat4 view = glm::translate(glm::mat4(1.f), camPos);
    glm::mat4 projection = glm::perspective(glm::radians(70.f), 800.f / 600.f, 0.1f, 200.0f);
    //projection[1][1] *= -1;
    glm::mat4 model = glm::mat4(1.f);

    PushConstant pushConstant;
    pushConstant.transform = projection * view * model;

    const auto& device = Context::Instance().GetDevice();
    const auto& pipeline = Context::Instance().GetPipeline();
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
        m_CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.GetPipeline());
        mesh->Bind(m_CommandBuffer);
        vk::RenderPassBeginInfo renderPassBegin;
        vk::Rect2D area;
        vk::ClearValue color, depth;
        area.setOffset({0, 0})
            .setExtent(swapchain.GetExtent());
        color.setColor({0.1f, 0.1f, 0.1f, 1.0f});
        depth.setDepthStencil(1.0f);
        std::array clearValues = {color, depth};
        renderPassBegin.setRenderPass(pipeline.GetRenderPass())
                       .setRenderArea(area)
                       .setFramebuffer(swapchain.GetFramebuffer(imageIndex))
                       .setClearValues(clearValues);
        m_CommandBuffer.beginRenderPass(renderPassBegin, {});
        m_CommandBuffer.pushConstants(pipeline.GetLayout(), vk::ShaderStageFlagBits::eVertex, 0, PushConstantSize(),
                                      &pushConstant);
        mesh->Draw(m_CommandBuffer);
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

void Renderer::OnUpdate(float deltaTime) const
{
    m_Camera->OnUpdate(deltaTime);
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
