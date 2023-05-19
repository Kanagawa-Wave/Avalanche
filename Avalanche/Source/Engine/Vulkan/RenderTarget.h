#pragma once

#include "API/Image.h"
#include "API/RenderPass.h"

class RenderTarget
{
public:
    RenderTarget(vk::Format format, vk::Extent2D extent, vk::Bool32 renderDepth);
    ~RenderTarget();

    vk::RenderPass GetRenderPass() const { return m_RenderPass->GetRenderPass(); }

    void Begin(vk::CommandBuffer commandBuffer) const;
    void End(vk::CommandBuffer commandBuffer) const;

private:
    std::unique_ptr<RenderPass> m_RenderPass;
    std::unique_ptr<Image> m_Image;
    vk::Framebuffer m_Framebuffer;

    vk::Extent2D m_Extent;
};
