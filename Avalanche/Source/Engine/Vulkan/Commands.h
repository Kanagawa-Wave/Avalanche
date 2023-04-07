#pragma once

#include <functional>
#include <vulkan/vulkan.hpp>

class Commands
{
public:
    static void Init();
    static void ImmediateSubmit(std::function<void(vk::CommandBuffer commandBuffer)>&& function);
    static void Shutdown();

private:
    struct ImmediateContext
    {
        vk::Fence fence;
        vk::CommandPool commandPool;
        vk::CommandBuffer commandBuffer;
    };
    
    static ImmediateContext s_ImmediateContext;
};
