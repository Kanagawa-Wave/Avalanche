#pragma once

#include <functional>
#include <vulkan/vulkan.hpp>

class ImmediateContext
{
public:
    static void Init();
    static void Submit(std::function<void(vk::CommandBuffer commandBuffer)>&& function);
    static void Shutdown();

private:
    static vk::Fence m_Fence;
    static vk::CommandPool m_CommandPool;
    static vk::CommandBuffer m_CommandBuffer;
};
