#pragma once

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

#include "RenderTarget.h"
#include "glm/fwd.hpp"
#include "Scene/Scene.h"
#include "Vulkan/Pipeline.h"

class ShadowMapRenderSystem
{
public:
    ShadowMapRenderSystem(uint32_t resolution);
    ~ShadowMapRenderSystem();

    void Render(const Scene& scene, vk::CommandBuffer commandBuffer);

private:
    struct ShadowMapPushConstant
    {
        glm::mat4 Model;
    } m_PushConstant{};

    struct ShadowMapUniformBuffer
    {
        glm::mat4 ViewProjection;
    } m_UniformBuffer{};
    
    std::unique_ptr<Pipeline> m_Pipeline;
    std::unique_ptr<RenderTarget> m_RenderTarget;
    uint32_t m_Resolution;
};
