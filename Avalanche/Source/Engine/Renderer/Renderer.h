#pragma once

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

#include "RenderTarget.h"

#include "Engine/Scene/Components/Camera.h"
#include "Engine/Scene/Components/Mesh.h"
#include "Vulkan/Pipeline.h"

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

    void Begin();
    void Render(const Mesh* mesh);
    void Update(float deltaTime);

private:
    void AllocateCommandBuffer();
    void CreateSemaphores();
    void CreateFence();
    void CreateDescriptorSets();
    void InitCamera(float aspect);
    void InitImGui();
    void OnImGuiUpdate();

private:
    Window* m_Window;
    bool m_EnableImGui = true;

    std::unique_ptr<RenderPass> m_PresnetRenderPass;
    std::unique_ptr<Pipeline> m_ViewportPipeline;

    std::unique_ptr<Pipeline> m_TestPipeline;
    std::unique_ptr<RenderTarget> m_TestRenderTarget;

    vk::DescriptorPool m_ImGuiPool;
    vk::CommandBuffer m_CommandBuffer;
    vk::Semaphore m_RenderSemaphore, m_PresentSemaphore;
    vk::Fence m_Fence;
    
    struct CameraData
    {
        glm::mat4 Projection{};
        glm::mat4 View{};
        glm::mat4 ViewProjection{};

        void SetData(const glm::mat4& projection, const glm::mat4& view)
        {
            this->Projection = projection;
            this->View = view;
            this->ViewProjection = projection * view;
        }
    } m_CameraData;

    struct TestData
    {
        float test = 1.f;
    } m_TestData;
    
    std::unique_ptr<Camera> m_Camera;
    std::unique_ptr<Buffer> m_CameraBuffer, m_TestBuffer;

    std::unique_ptr<DescriptorSet> m_GlobalSet, m_TextureSet;
};
