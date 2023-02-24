#include "Initializer.h"

vk::CommandPoolCreateInfo Initializer::CommandPool(uint32_t queueFamilyIndex, vk::CommandPoolCreateFlagBits flags)
{
    vk::CommandPoolCreateInfo commandPoolInfo;
    commandPoolInfo.setQueueFamilyIndex(queueFamilyIndex)
                   .setFlags(flags);

    return commandPoolInfo;
}

vk::CommandBufferAllocateInfo Initializer::CommandBuffer(vk::CommandPool commandPool, uint32_t count,
                                                         vk::CommandBufferLevel level)
{
    vk::CommandBufferAllocateInfo commandBufferInfo;
    commandBufferInfo.setCommandPool(commandPool)
                     .setCommandBufferCount(count)
                     .setLevel(level);

    return commandBufferInfo;
}
