#pragma once

#include "Texture.h"
#include "Vulkan/RenderPass.h"

class RenderTarget
{
public:
    RenderTarget(vk::Format format, vk::Extent2D extent, vk::Bool32 renderDepth);
    ~RenderTarget();

    vk::RenderPass GetRenderPass() const { return m_RenderPass->GetRenderPass(); }
    Texture* GetRenderTexture() const { return m_Texture.get(); }
    vk::Extent2D GetExtent() const { return m_Extent; }

    void Begin(vk::CommandBuffer commandBuffer) const;
    void End(vk::CommandBuffer commandBuffer) const;

private:
    std::unique_ptr<RenderPass> m_RenderPass;
    std::unique_ptr<Texture> m_Texture;
    vk::Framebuffer m_Framebuffer;

    vk::Extent2D m_Extent;
};
