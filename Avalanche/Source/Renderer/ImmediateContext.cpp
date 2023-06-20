#include "ImmediateContext.h"

#include "Context.h"
#include "CommandManager.h"
#include "Core/Log.h"

vk::Fence ImmediateContext::m_Fence;
vk::CommandBuffer ImmediateContext::m_CommandBuffer;
std::unique_ptr<CommandManager> ImmediateContext::m_CommandManager = nullptr;

void ImmediateContext::Init()
{
    const auto& device = Context::Instance().GetDevice();
    vk::FenceCreateInfo fenceInfo;
    fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
    m_Fence = device.createFence(fenceInfo);

    m_CommandManager = std::make_unique<CommandManager>();
    m_CommandBuffer = m_CommandManager->AllocateCommandBuffer();
}

void ImmediateContext::Submit(std::function<void(vk::CommandBuffer commandBuffer)>&& function)
{
    const auto& ctx = Context::Instance();
    const auto& device = Context::Instance().GetDevice();

    m_CommandManager->ResetCommands();
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
    const auto& device = Context::Instance().GetDevice();

    device.destroyFence(m_Fence);
    m_CommandManager.reset();
}

void ImmediateContext::Begin()
{
    const auto& device = Context::Instance().GetDevice();

    m_CommandManager->ResetCommands();
    device.resetFences(m_Fence);

    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    m_CommandBuffer.begin(beginInfo);
}

void ImmediateContext::End()
{
    const auto& ctx = Context::Instance();
    const auto& device = Context::Instance().GetDevice();
    
    m_CommandBuffer.end();

    vk::SubmitInfo submitInfo;
    submitInfo.setCommandBuffers(m_CommandBuffer);
    ctx.GetGraphicsQueue().submit(submitInfo, m_Fence);

    if (device.waitForFences(m_Fence, true, std::numeric_limits<std::uint64_t>::max()) != vk::Result::eSuccess)
    {
        ASSERT(0, "Failed to wait for fence")
    }
}
