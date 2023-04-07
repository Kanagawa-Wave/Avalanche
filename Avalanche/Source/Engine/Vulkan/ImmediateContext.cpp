#include "ImmediateContext.h"

#include "Context.h"

#include <thread>

vk::Fence ImmediateContext::m_Fence;
vk::CommandPool ImmediateContext::m_CommandPool;
vk::CommandBuffer ImmediateContext::m_CommandBuffer;

void ImmediateContext::Init()
{
    auto& ctx = Context::Instance();
    auto& device = Context::Instance().GetDevice();

    vk::FenceCreateInfo fenceInfo;
    fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
    m_Fence = device.createFence(fenceInfo);

    vk::CommandPoolCreateInfo poolInfo;
    poolInfo.setQueueFamilyIndex(ctx.GetGraphicsQueueFamilyIndex());
    m_CommandPool = device.createCommandPool(poolInfo);

    vk::CommandBufferAllocateInfo allocateInfo;
    allocateInfo.setLevel(vk::CommandBufferLevel::ePrimary)
                .setCommandPool(m_CommandPool)
                .setCommandBufferCount(1);

    m_CommandBuffer = device.allocateCommandBuffers(allocateInfo)[0];
}

void ImmediateContext::Submit(std::function<void(vk::CommandBuffer commandBuffer)>&& function)
{
    auto& ctx = Context::Instance();
    auto& device = Context::Instance().GetDevice();

    device.resetCommandPool(m_CommandPool);
    device.resetFences(m_Fence);
    
    vk::CommandBuffer commandBuffer = m_CommandBuffer;

    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    commandBuffer.begin(beginInfo);
    function(commandBuffer);
    commandBuffer.end();

    vk::SubmitInfo submitInfo;
    submitInfo.setCommandBuffers(commandBuffer);
    ctx.GetGraphicsQueue().submit(submitInfo, m_Fence);

    if (device.waitForFences(m_Fence, true, std::numeric_limits<std::uint64_t>::max()) != vk::Result::eSuccess)
    {
        ASSERT(0, "Failed to wait for fence")
    }
}

void ImmediateContext::Shutdown()
{
    auto& device = Context::Instance().GetDevice();

    device.destroyFence(m_Fence);
    device.destroyCommandPool(m_CommandPool);
}
