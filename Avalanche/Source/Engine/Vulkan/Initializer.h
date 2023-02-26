#pragma once

#include "Types.h"

class Initializer
{
public:
    static vk::CommandPoolCreateInfo CommandPoolCreate(uint32_t queueFamilyIndex, vk::CommandPoolCreateFlagBits flags);
    static vk::CommandBufferAllocateInfo CommandBufferAllocate(vk::CommandPool commandPool, uint32_t count = 1,
                                                       vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary);
    static vk::CommandBufferBeginInfo CommandBufferBegin(vk::CommandBufferUsageFlagBits flags);
    static vk::FramebufferCreateInfo FramebufferCreate(vk::RenderPass renderPass, vk::Extent2D extent);
    static vk::FenceCreateInfo FenceCreate(vk::FenceCreateFlagBits flags);
    static vk::SemaphoreCreateInfo SemaphoreCreate(vk::SemaphoreCreateFlagBits flags = vk::SemaphoreCreateFlagBits());
    static vk::SubmitInfo SubmitInfo(vk::CommandBuffer& commandBuffer);
    static vk::PresentInfoKHR PresentInfo();
    static vk::RenderPassBeginInfo RenderpassBegin(vk::RenderPass renderPass, vk::Extent2D extent, vk::Framebuffer framebuffer);
};
