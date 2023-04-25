﻿#pragma once

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

#include "API/Pipeline.h"
#include "API/RenderPass.h"
#include "Engine/Scene/Components/Camera.h"
#include "Engine/Scene/Components/Mesh.h"

class Window;

struct PushConstant
{
    glm::mat4 model{1.0};
};

class Renderer
{
public:
    Renderer(Window* window, bool enableImGui);
    ~Renderer();
    
    void Render(const Mesh* mesh);
    void OnUpdate(float deltaTime);
    
    static uint32_t PushConstantSize() { return sizeof(PushConstant); }

private:
    void AllocateCommandBuffer();
    void CreateSemaphores();
    void CreateFence();
    void InitCamera(float aspect);
    void InitImGui();
    void OnImGuiUpdate();
    void InitImGUIObjects();

private:
    Window* m_Window;
    bool m_EnableImGui = true;

    std::unique_ptr<RenderPass> m_MainRenderPass, m_ViewportRenderPass;
    std::unique_ptr<Pipeline> m_MainPipeline, m_ViewportPipeline;
    
    vk::CommandPool m_CommandPool;
    vk::CommandBuffer m_CommandBuffer;
    vk::Semaphore m_RenderSemaphore, m_PresentSemaphore;
    vk::Fence m_Fence;

    struct ImGuiObjects
    {
        std::vector<vk::Image> m_ViewportImages;
        std::vector<VmaAllocation> m_Allocations;
        std::vector<vk::ImageView> m_ViewportImageViews;
        std::vector<vk::Framebuffer> m_ViewportFramebuffers;
        vk::DescriptorPool ImGuiPool;
    } m_ImGuiData;
    
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

    struct TestData
    {
        float test = 1.f;
    } m_TestData;
    
    std::unique_ptr<Camera> m_Camera;
    std::unique_ptr<Buffer> m_CameraBuffer, m_TestBuffer;
};
