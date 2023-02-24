#pragma once

#include "Types.h"

class Initializer
{
public:
    static vk::CommandPoolCreateInfo CommandPool(uint32_t queueFamilyIndex, vk::CommandPoolCreateFlagBits flags);
    static vk::CommandBufferAllocateInfo CommandBuffer(vk::CommandPool commandPool, uint32_t count = 1,
                                                       vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary);
};
