#include "Commands.h"

#include "Context.h"

Commands::ImmediateContext Commands::s_ImmediateContext;

void Commands::Init()
{
    auto& ctx = Context::Instance();
    auto& device = Context::Instance().GetDevice();

    vk::FenceCreateInfo fenceInfo;
    fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
    s_ImmediateContext.fence = device.createFence(fenceInfo);

    vk::CommandPoolCreateInfo poolInfo;
    poolInfo.setQueueFamilyIndex(ctx.GetGraphicsQueueFamilyIndex());
    s_ImmediateContext.commandPool = device.createCommandPool(poolInfo);

    vk::CommandBufferAllocateInfo allocateInfo;
    allocateInfo.setLevel(vk::CommandBufferLevel::ePrimary)
                .setCommandPool(s_ImmediateContext.commandPool)
                .setCommandBufferCount(1);

    s_ImmediateContext.commandBuffer = device.allocateCommandBuffers(allocateInfo)[0];
}

void Commands::ImmediateSubmit(std::function<void(vk::CommandBuffer commandBuffer)>&& function)
{
    auto& ctx = Context::Instance();
    auto& device = Context::Instance().GetDevice();

    device.resetFences(s_ImmediateContext.fence);
    
    vk::CommandBuffer commandBuffer = s_ImmediateContext.commandBuffer;

    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    commandBuffer.begin(beginInfo);
    function(commandBuffer);
    commandBuffer.end();

    vk::SubmitInfo submitInfo;
    submitInfo.setCommandBuffers(commandBuffer);
    ctx.GetGraphicsQueue().submit(submitInfo, s_ImmediateContext.fence);

    if (device.waitForFences(s_ImmediateContext.fence, true, std::numeric_limits<std::uint64_t>::max()) != vk::Result::eSuccess)
    {
        ASSERT(0, "Failed to wait for fence")
    }
    device.resetFences(s_ImmediateContext.fence);
    device.resetCommandPool(s_ImmediateContext.commandPool);
}

void Commands::Shutdown()
{
    auto& device = Context::Instance().GetDevice();

    device.destroyFence(s_ImmediateContext.fence);
    device.destroyCommandPool(s_ImmediateContext.commandPool);
}
