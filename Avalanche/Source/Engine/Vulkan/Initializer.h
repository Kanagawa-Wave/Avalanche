#pragma once

#include <vulkan/vulkan.hpp>

class Initializer
{
public:
    static vk::CommandPoolCreateInfo CommandPool(uint32_t queueFamilyIndex, vk::CommandPoolCreateFlagBits flags);
    static vk::CommandBufferAllocateInfo CommandBufferAllocate(vk::CommandPool commandPool, uint32_t count = 1,
                                                       vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary);
    static vk::CommandBufferBeginInfo CommandBufferBegin(vk::CommandBufferUsageFlagBits flags);
    static vk::FramebufferCreateInfo Framebuffer(vk::RenderPass renderPass, vk::Extent2D extent);
    static vk::FenceCreateInfo Fence(vk::FenceCreateFlagBits flags);
    static vk::SemaphoreCreateInfo Semaphore(vk::SemaphoreCreateFlagBits flags = vk::SemaphoreCreateFlagBits());
    static vk::SubmitInfo Submit(vk::CommandBuffer& commandBuffer);
    static vk::PresentInfoKHR Present();
    static vk::RenderPassBeginInfo RenderpassBegin(vk::RenderPass renderPass, vk::Extent2D extent, vk::Framebuffer framebuffer);
    static vk::PipelineShaderStageCreateInfo PipelineShaderStage(vk::ShaderStageFlagBits stage, vk::ShaderModule shaderModule);
    static vk::PipelineVertexInputStateCreateInfo PipelineVertexInput();
    static vk::PipelineInputAssemblyStateCreateInfo PipelineInputAssembly(vk::PrimitiveTopology topology);
    static vk::PipelineRasterizationStateCreateInfo PipelineRasterization(vk::PolygonMode polygonMode);
    static vk::PipelineMultisampleStateCreateInfo PipelineMultisample();
    static vk::PipelineColorBlendAttachmentState PipelineColorBlendAttachment();
    static vk::PipelineLayoutCreateInfo PipelineLayout();
};
