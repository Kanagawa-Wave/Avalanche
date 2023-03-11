#pragma once

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

#include "Engine/Scene/Components/Mesh.h"

struct PushConstant
{
    glm::mat4 transform{1.0};
};

class Renderer
{
public:
    Renderer();
    ~Renderer();

    void Render(Mesh* mesh);
    
    static uint32_t PushConstantSize() { return sizeof(PushConstant); }

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
