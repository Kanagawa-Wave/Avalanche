#include "Initializer.h"

vk::CommandPoolCreateInfo Initializer::CommandPoolCreate(uint32_t queueFamilyIndex, vk::CommandPoolCreateFlagBits flags)
{
    vk::CommandPoolCreateInfo commandPoolInfo;
    commandPoolInfo.setQueueFamilyIndex(queueFamilyIndex)
                   .setFlags(flags);

    return commandPoolInfo;
}

vk::CommandBufferAllocateInfo Initializer::CommandBufferAllocate(vk::CommandPool commandPool, uint32_t count,
                                                                 vk::CommandBufferLevel level)
{
    vk::CommandBufferAllocateInfo commandBufferInfo;
    commandBufferInfo.setCommandPool(commandPool)
                     .setCommandBufferCount(count)
                     .setLevel(level);

    return commandBufferInfo;
}

vk::CommandBufferBeginInfo Initializer::CommandBufferBegin(vk::CommandBufferUsageFlagBits flags)
{
    vk::CommandBufferBeginInfo commandBufferInfo;
    commandBufferInfo.setFlags(flags);

    return commandBufferInfo;
}

vk::FramebufferCreateInfo Initializer::FramebufferCreate(vk::RenderPass renderPass, vk::Extent2D extent)
{
    vk::FramebufferCreateInfo framebufferInfo;
    framebufferInfo.setRenderPass(renderPass)
                   .setAttachmentCount(1)
                   .setWidth(extent.width)
                   .setHeight(extent.height)
                   .setLayers(1);

    return framebufferInfo;
}

vk::FenceCreateInfo Initializer::FenceCreate(vk::FenceCreateFlagBits flags)
{
    vk::FenceCreateInfo fenceInfo;
    fenceInfo.setFlags(flags);

    return fenceInfo;
}

vk::SemaphoreCreateInfo Initializer::SemaphoreCreate(vk::SemaphoreCreateFlagBits flags)
{
    vk::SemaphoreCreateInfo semaphoreInfo;
    semaphoreInfo.setFlags(flags);

    return semaphoreInfo;
}

vk::SubmitInfo Initializer::SubmitInfo(vk::CommandBuffer& commandBuffer)
{
    vk::SubmitInfo submitInfo;
    submitInfo.setCommandBuffers(commandBuffer)
              .setWaitSemaphores(nullptr)
              .setSignalSemaphores(nullptr);

    return submitInfo;
}

vk::PresentInfoKHR Initializer::PresentInfo()
{
    vk::PresentInfoKHR presentInfo;
    presentInfo.setSwapchains(nullptr)
               .setWaitSemaphores(nullptr)
               .setImageIndices(nullptr);

    return presentInfo;
}

vk::RenderPassBeginInfo Initializer::RenderpassBegin(vk::RenderPass renderPass, vk::Extent2D extent,
                                                     vk::Framebuffer framebuffer)
{
    vk::RenderPassBeginInfo renderPassInfo;
    renderPassInfo.setFramebuffer(framebuffer)
                  .setRenderPass(renderPass)
                  .setRenderArea(vk::Rect2D({0, 0}, extent))
                  .setClearValues(nullptr);

    return renderPassInfo;
}
