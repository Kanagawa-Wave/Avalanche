#include "Initializer.h"

vk::CommandPoolCreateInfo Initializer::CommandPool(uint32_t queueFamilyIndex, vk::CommandPoolCreateFlagBits flags)
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

vk::FramebufferCreateInfo Initializer::Framebuffer(vk::RenderPass renderPass, vk::Extent2D extent)
{
    vk::FramebufferCreateInfo framebufferInfo;
    framebufferInfo.setRenderPass(renderPass)
                   .setAttachmentCount(1)
                   .setWidth(extent.width)
                   .setHeight(extent.height)
                   .setLayers(1);

    return framebufferInfo;
}

vk::FenceCreateInfo Initializer::Fence(vk::FenceCreateFlagBits flags)
{
    vk::FenceCreateInfo fenceInfo;
    fenceInfo.setFlags(flags);

    return fenceInfo;
}

vk::SemaphoreCreateInfo Initializer::Semaphore(vk::SemaphoreCreateFlagBits flags)
{
    vk::SemaphoreCreateInfo semaphoreInfo;
    semaphoreInfo.setFlags(flags);

    return semaphoreInfo;
}

vk::SubmitInfo Initializer::Submit(vk::CommandBuffer& commandBuffer)
{
    vk::SubmitInfo submitInfo;
    submitInfo.setCommandBuffers(commandBuffer)
              .setWaitSemaphores(nullptr)
              .setSignalSemaphores(nullptr);

    return submitInfo;
}

vk::PresentInfoKHR Initializer::Present()
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

vk::PipelineShaderStageCreateInfo Initializer::PipelineShaderStage(vk::ShaderStageFlagBits stage,
                                                                   vk::ShaderModule shaderModule)
{
    vk::PipelineShaderStageCreateInfo pipelineShaderInfo;
    pipelineShaderInfo.setStage(stage)
                      .setModule(shaderModule)
                      .setPName("main");

    return pipelineShaderInfo;
}

vk::PipelineVertexInputStateCreateInfo Initializer::PipelineVertexInput()
{
    vk::PipelineVertexInputStateCreateInfo pipelineVertexInfo;

    return pipelineVertexInfo;
}

vk::PipelineInputAssemblyStateCreateInfo Initializer::PipelineInputAssembly(vk::PrimitiveTopology topology)
{
    vk::PipelineInputAssemblyStateCreateInfo pipelineAssemblyInfo;
    pipelineAssemblyInfo.setTopology(topology)
                        .setPrimitiveRestartEnable(false);

    return pipelineAssemblyInfo;
}

vk::PipelineRasterizationStateCreateInfo Initializer::PipelineRasterization(vk::PolygonMode polygonMode)
{
    vk::PipelineRasterizationStateCreateInfo pipelineRasterizationInfo;
    pipelineRasterizationInfo.setDepthClampEnable(false)
                             .setRasterizerDiscardEnable(false)
                             .setPolygonMode(polygonMode)
                             .setLineWidth(1.f)
                             .setCullMode(vk::CullModeFlagBits::eNone)
                             .setFrontFace(vk::FrontFace::eClockwise)
                             .setDepthBiasEnable(false)
                             .setDepthBiasConstantFactor(0.f)
                             .setDepthBiasClamp(0.f)
                             .setDepthBiasSlopeFactor(0.f);

    return pipelineRasterizationInfo;
}

vk::PipelineMultisampleStateCreateInfo Initializer::PipelineMultisample()
{
    vk::PipelineMultisampleStateCreateInfo pipelineMultisampleInfo;
    pipelineMultisampleInfo.setSampleShadingEnable(false);

    return pipelineMultisampleInfo;
}

vk::PipelineColorBlendAttachmentState Initializer::PipelineColorBlendAttachment()
{
    vk::PipelineColorBlendAttachmentState pipelineColorBlendAttachment;
    pipelineColorBlendAttachment.setColorWriteMask(
                                    vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                    vk::ColorComponentFlagBits::eB |
                                    vk::ColorComponentFlagBits::eA)
                                .setBlendEnable(false);

    return pipelineColorBlendAttachment;
}

vk::PipelineLayoutCreateInfo Initializer::PipelineLayout()
{
    vk::PipelineLayoutCreateInfo pipelineLayoutInfo;

    return pipelineLayoutInfo;
}
