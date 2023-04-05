﻿#pragma once

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

#include "Engine/Scene/Components/Camera.h"
#include "Engine/Scene/Components/Mesh.h"

struct PushConstant
{
    glm::mat4 model{1.0};
};

class Renderer
{
public:
    Renderer(float aspect);
    ~Renderer();

    void Init();
    void Render(const Mesh& mesh);
    void OnUpdate(float deltaTime) const;
    
    static uint32_t PushConstantSize() { return sizeof(PushConstant); }

private:
    void AllocateCommandBuffer();
    void CreateSemaphores();
    void CreateFence();
    void InitCamera(float aspect);

private:
    float m_Aspect;
    
    vk::CommandPool m_CommandPool;
    vk::CommandBuffer m_CommandBuffer;
    vk::Semaphore m_RenderSemaphore, m_PresentSemaphore;
    vk::Fence m_Fence;

    struct CameraData
    {
        glm::mat4 projection{};
        glm::mat4 view{};
        glm::mat4 viewProjection{};

        void SetData(const glm::mat4& projection, const glm::mat4 view)
        {
            this->projection = projection;
            this->view = view;
            this->viewProjection = projection * view;
        }
    } m_CameraData;
    
    std::unique_ptr<Camera> m_Camera;
    std::unique_ptr<Buffer> m_CameraBuffer;
};
