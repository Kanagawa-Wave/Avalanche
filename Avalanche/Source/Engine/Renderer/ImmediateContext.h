#pragma once

#include <functional>
#include <vulkan/vulkan.hpp>

class CommandManager;

class ImmediateContext
{
public:
    static void Init();
    static void Submit(std::function<void(vk::CommandBuffer commandBuffer)>&& function);
    static void Shutdown();

    static void Begin();
    static void End();

    static vk::CommandBuffer GetCommandBuffer() { return m_CommandBuffer; }

private:
    static vk::Fence m_Fence;
    static vk::CommandBuffer m_CommandBuffer;
    static std::unique_ptr<CommandManager> m_CommandManager;
};
