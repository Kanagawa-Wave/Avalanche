#pragma once

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

#include "RenderTarget.h"
#include "ShadowMapRenderSystem.h"
#include "Scene/Scene.h"
#include "Scene/Components/Components.h"

#include "Vulkan/Pipeline.h"

struct TransformComponent;
class Window;

struct MainPushConstant
{
    glm::mat4 model{};
    glm::mat4 normalMat{};
};

struct BillboardPushConstant
{
    glm::vec3 position{};
};

class Renderer
{
public:
    Renderer() = default;
    Renderer(Window* window, const vk::Extent2D& viewportExtent);
    ~Renderer();

    bool Begin(const Camera& camera, const Scene& scene);
    void End();
    void Render(const Camera& camera, const Scene& scene);

    void OnResize(vk::Extent2D extent) const;
    void* GetViewportTextureID() const;

private:
    void DrawModel(const TransformComponent& transform, const StaticMeshComponent& mesh) const;
    void DrawBillboard(const TransformComponent& transform, const BillboardComponent& billboard) const;
    void AllocateCommandBuffer();
    void CreateSemaphores();
    void CreateFence();
    void InitImGui();
    
private:
    Window* m_Window = nullptr;

    uint32_t m_ImageIndex = 0;

    std::unique_ptr<ShadowMapRenderSystem> m_ShadowMapRenderSystem;

    std::unique_ptr<RenderPass> m_PresnetRenderPass = nullptr;
    std::unique_ptr<Pipeline> m_UIPipeline = nullptr;

    std::unique_ptr<Pipeline> m_MainPipeline = nullptr, m_BillboardPipeline = nullptr;
    vk::DescriptorSet m_MainGlobalSet = VK_NULL_HANDLE, m_BillboardGlobalSet = VK_NULL_HANDLE;
    std::unique_ptr<DescriptorSetWriter> m_DescriptorSetWriter = nullptr;
    std::unique_ptr<RenderTarget> m_ViewportRenderTarget = nullptr;

    vk::DescriptorPool m_ImGuiPool = VK_NULL_HANDLE;
    vk::CommandBuffer m_CommandBuffer = VK_NULL_HANDLE;
    vk::Semaphore m_RenderSemaphore = VK_NULL_HANDLE, m_PresentSemaphore = VK_NULL_HANDLE;
    vk::Fence m_Fence = VK_NULL_HANDLE;

    std::vector<vk::DescriptorSetLayout> m_DescriptorSetLayouts = {};
    
    struct CameraDataVert
    {
        glm::mat4 Projection{};
        glm::mat4 View{};
        glm::mat4 ViewProjection{};

        void SetData(const glm::mat4& projection, const glm::mat4& view)
        {
            Projection = projection;
            View = view;
            ViewProjection = projection * view;
        }
    } m_CameraDataVert;

    struct CameraDataFrag
    {
	    alignas(16) glm::vec3 Position{};

        void SetData(const glm::vec3& position)
        {
	        Position = position;
        }
    } m_CameraDataFrag;

    struct PointLightData
    {
        struct PointLight
        {
            alignas(16) glm::vec3 Position{};
            alignas(16) glm::vec3 Color{};
        } m_PointLights[16];
        uint32_t m_PointLightCount = 0;

        void SetData(const glm::vec3& position, const glm::vec3& color)
        {
	        m_PointLights[m_PointLightCount].Position = position;
            m_PointLights[m_PointLightCount].Color = color;
            m_PointLightCount++;
        }
    } m_PointLightData;
    std::unique_ptr<Buffer> m_CameraVertUniformBuffer, m_PointLightUniformBuffer, m_CameraFragUniformBuffer = nullptr;

    const vk::Extent2D* m_pExtent = nullptr;
};
