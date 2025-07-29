#pragma once

#include <functional>
#include <vulkan/vulkan.hpp>

class CommandManager;

class ImmediateContext
{
public:
    static void Init();
    static ImmediateContext& Instance();
    void Submit(std::function<void(vk::CommandBuffer commandBuffer)>&& function) const;
    void Destroy();

    void Begin() const;
    void End();

    vk::CommandBuffer GetCommandBuffer() const { return m_CommandBuffer; }

private:
    ImmediateContext();
    static std::unique_ptr<ImmediateContext> s_Instance;
    
    vk::Fence m_Fence;
    vk::CommandBuffer m_CommandBuffer;
    std::unique_ptr<CommandManager> m_CommandManager;
};
