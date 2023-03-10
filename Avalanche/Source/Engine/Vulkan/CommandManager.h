#pragma once

#include <vulkan/vulkan.hpp>

class CommandManager
{
public:
    CommandManager();
    ~CommandManager();

    vk::CommandBuffer AllocateCommandBuffer() const;
    std::vector<vk::CommandBuffer> AllocateCommandBuffers(uint32_t count) const;

    void ResetCommands() const;
    void FreeCommands(vk::ArrayProxy<vk::CommandBuffer> commandBuffers) const;

private:
    void CreateCommandPool();
    
private:
    vk::CommandPool m_CommandPool;
};
