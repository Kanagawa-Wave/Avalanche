#pragma once

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

#include "RenderTarget.h"
#include "Scene/Scene.h"
#include "Scene/Components/Components.h"

#include "Vulkan/Pipeline.h"

struct TransformComponent;
class Window;

struct PushConstant
{
    glm::mat4 model{};
    glm::mat4 normalMat{};
};

class Renderer
{
public:
    Renderer() = default;
    Renderer(Window* window, const vk::Extent2D& viewportExtent);
    ~Renderer();

    void Begin(const Camera& camera, const Scene& scene);
    void DrawModel(const TransformComponent& transform, const StaticMeshComponent& mesh) const;
    void End();
    void Render(const Camera& camera, const Scene& scene);

    void OnResize(vk::Extent2D extent) const;
    void* GetViewportTextureID() const;

private:
    void AllocateCommandBuffer();
    void CreateSemaphores();
    void CreateFence();
    void CreateDescriptorSets();
    void InitImGui();

private:
    Window* m_Window = nullptr;

    uint32_t m_ImageIndex = 0;

    std::unique_ptr<RenderPass> m_PresnetRenderPass = nullptr;
    std::unique_ptr<Pipeline> m_Pipeline = nullptr;

    std::unique_ptr<Pipeline> m_ViewportPipeline = nullptr;
    std::unique_ptr<RenderTarget> m_ViewportRenderTarget = nullptr;

    vk::DescriptorPool m_ImGuiPool = VK_NULL_HANDLE;
    vk::CommandBuffer m_CommandBuffer = VK_NULL_HANDLE;
    vk::Semaphore m_RenderSemaphore = VK_NULL_HANDLE, m_PresentSemaphore = VK_NULL_HANDLE;
    vk::Fence m_Fence = VK_NULL_HANDLE;
    
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

    struct PointLightData
    {
        glm::vec3 Position{};
        glm::vec3 Color{};
    } m_PointLightData;

    const vk::Extent2D* m_pExtent = nullptr;
    std::unique_ptr<Buffer> m_CameraBuffer = nullptr, m_PointLightBuffer = nullptr;

    std::unique_ptr<DescriptorSet> m_GlobalSet = nullptr, m_TextureSet = nullptr;

    struct DrawableObject
    {
        const Mesh* pMesh = nullptr;
        const TransformComponent* pTransform = nullptr;
    };
    std::vector<DrawableObject> m_DrawList{};
};
