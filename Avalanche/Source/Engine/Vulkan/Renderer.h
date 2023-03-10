#pragma once

#include <vulkan/vulkan.hpp>

class Renderer
{
public:
    Renderer();
    ~Renderer();

    void Render();

private:
    void AllocateCommandBuffer();
    void CreateSemaphores();
    void CreateFence();
    
private:
    vk::CommandPool m_CommandPool;
    vk::CommandBuffer m_CommandBuffer;
    vk::Semaphore m_RenderSemaphore, m_PresentSemaphore;
    vk::Fence m_Fence;
};
