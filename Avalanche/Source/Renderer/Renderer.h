#pragma once

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

#include "RenderTarget.h"
#include "Scene/Scene.h"

#include "Scene/Components/Camera.h"
#include "Scene/Components/Mesh.h"
#include "Vulkan/Pipeline.h"

struct TransformComponent;
class Window;

struct PushConstant
{
    glm::mat4 model{1.0};
};

class Renderer
{
public:
    Renderer(Window* window, const Camera* camera, const vk::Extent2D& viewportExtent);
    ~Renderer();
    
    void SubmitScene(Scene* scene);
    void OnRender();

    void ResizeViewport(vk::Extent2D extent) const;
    void* GetViewportTextureID() const;

private:
    void AllocateCommandBuffer();
    void CreateSemaphores();
    void CreateFence();
    void CreateDescriptorSets();
    void InitImGui();

private:
    Window* m_Window;

    std::unique_ptr<RenderPass> m_PresnetRenderPass;
    std::unique_ptr<Pipeline> m_Pipeline;

    std::unique_ptr<Pipeline> m_ViewportPipeline;
    std::unique_ptr<RenderTarget> m_ViewportRenderTarget;

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
    
    const Camera* m_pCamera = nullptr;
    const vk::Extent2D* m_pExtent = nullptr;
    std::unique_ptr<Buffer> m_CameraBuffer, m_TestBuffer;

    std::unique_ptr<DescriptorSet> m_GlobalSet, m_TextureSet;

    struct DrawableObject
    {
        const Mesh* pMesh = nullptr;
        const TransformComponent* pTransform = nullptr;
    };
    std::vector<DrawableObject> m_DrawList;
};
