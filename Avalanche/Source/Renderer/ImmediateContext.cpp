#include "ImmediateContext.h"

#include "Context.h"
#include "CommandManager.h"
#include "Core/Input.h"
#include "Core/Log.h"

std::unique_ptr<ImmediateContext> ImmediateContext::s_Instance = nullptr;

void ImmediateContext::Init()
{
    s_Instance.reset(new ImmediateContext());
}

ImmediateContext& ImmediateContext::Instance()
{
    return *s_Instance;
}

void ImmediateContext::Submit(std::function<void(vk::CommandBuffer commandBuffer)>&& function) const
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

void ImmediateContext::Destroy()
{
    const auto& device = Context::Instance().GetDevice();

    LOG_T("Destroying VkFence {}", fmt::ptr((VkFence)m_Fence))
    device.destroyFence(m_Fence);
 
    m_CommandManager.reset();
}

void ImmediateContext::Begin() const
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

ImmediateContext::ImmediateContext()
{
    const auto& device = Context::Instance().GetDevice();
    vk::FenceCreateInfo fenceInfo;
    fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
    m_Fence = device.createFence(fenceInfo);
    LOG_T("VkFence {0} created successfully", fmt::ptr((VkFence)m_Fence))

    m_CommandManager = std::make_unique<CommandManager>();
    m_CommandBuffer = m_CommandManager->AllocateCommandBuffer();
}
