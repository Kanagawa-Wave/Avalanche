#pragma once

#include "Texture.h"
#include "Vulkan/RenderPass.h"

class RenderTarget
{
public:
    RenderTarget(vk::Format format, vk::Extent2D extent, vk::Bool32 renderDepth);
    ~RenderTarget();

    vk::RenderPass GetRenderPass() const { return m_RenderPass->GetRenderPass(); }
    Texture* GetRenderTexture() const { return m_RenderTexture.get(); }
    vk::Extent2D GetExtent() const { return m_Extent; }

    void Begin(vk::CommandBuffer commandBuffer) const;
    void End(vk::CommandBuffer commandBuffer) const;
    void Resize(vk::Extent2D extent);

private:
    std::unique_ptr<RenderPass> m_RenderPass;
    std::unique_ptr<Texture> m_RenderTexture, m_DepthTexture;
    vk::Framebuffer m_Framebuffer;

    vk::Format m_Format;
    vk::Extent2D m_Extent;
    vk::Bool32 m_RenderDepth;
};
