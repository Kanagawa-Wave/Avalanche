#include "CommandManager.h"

#include "Context.h"

CommandManager::CommandManager()
{
    CreateCommandPool();
}

CommandManager::~CommandManager()
{
    const auto& ctx = Context::Instance();
    LOG_T("Destroying VkCommandPool {}", fmt::ptr((VkCommandPool)m_CommandPool))
    ctx.GetDevice().destroyCommandPool(m_CommandPool);
}

vk::CommandBuffer CommandManager::AllocateCommandBuffer() const
{
    return AllocateCommandBuffers(1).front();
}

std::vector<vk::CommandBuffer> CommandManager::AllocateCommandBuffers(uint32_t count) const
{
    const auto& ctx = Context::Instance();
    vk::CommandBufferAllocateInfo commandBuffer;
    commandBuffer.setCommandBufferCount(count);
    commandBuffer.setCommandPool(m_CommandPool);
    commandBuffer.setLevel(vk::CommandBufferLevel::ePrimary);
    return ctx.GetDevice().allocateCommandBuffers(commandBuffer);
}

void CommandManager::ResetCommands() const
{
    Context::Instance().GetDevice().resetCommandPool(m_CommandPool);
}

void CommandManager::FreeCommands(vk::ArrayProxy<vk::CommandBuffer> commandBuffers) const
{
    Context::Instance().GetDevice().freeCommandBuffers(m_CommandPool, commandBuffers);
}

void CommandManager::CreateCommandPool()
{
    const auto& ctx = Context::Instance();
    vk::CommandPoolCreateInfo commandPool;
    commandPool.setQueueFamilyIndex(ctx.GetGraphicsQueueFamilyIndex())
               .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
    m_CommandPool = ctx.GetDevice().createCommandPool(commandPool);
    LOG_T("VkCommandPool {0} created successfully", fmt::ptr((VkCommandPool)m_CommandPool))
}
