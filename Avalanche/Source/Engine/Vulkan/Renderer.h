#pragma once

#include <vulkan/vulkan.hpp>

#include "Engine/Scene/Components/Mesh.h"

class Renderer
{
public:
    Renderer();
    ~Renderer();

    void Render(Mesh* mesh);

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
