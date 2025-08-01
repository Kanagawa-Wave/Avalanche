#pragma once

#include "Texture.h"
#include "Vulkan/RenderPass.h"

struct RenderTargetCreateInfo
{
    vk::Format ColorFormat;
    vk::Extent2D Extent;
    vk::Bool32 RenderDepth, RenderColor, ImGuiReadable;

    RenderTargetCreateInfo& setColorFormat(vk::Format format)
    {
        ColorFormat = format;
        if (format == vk::Format::eUndefined)
            RenderDepth = false;
        return *this;
    }

    RenderTargetCreateInfo& setExtent(vk::Extent2D extent)
    {
        Extent = extent;
        return *this;
    }

    RenderTargetCreateInfo& setRenderDepth(bool renderDepth)
    {
        RenderDepth = renderDepth;
        return *this;
    }

    RenderTargetCreateInfo& setRenderColor(bool renderColor)
    {
        RenderColor = renderColor;
        return *this;
    }

    RenderTargetCreateInfo& setImGuiReadable(bool readable)
    {
        ImGuiReadable = readable;
        return *this;
    }
};

class RenderTarget
{
public:
    RenderTarget(const RenderTargetCreateInfo& info);
    ~RenderTarget();

    vk::RenderPass GetRenderPass() const { return m_RenderPass->GetRenderPass(); }
    Texture* GetRenderTexture() const { return m_ColorTexture.get(); }
    vk::Extent2D GetExtent() const { return m_Extent; }

    void Begin(vk::CommandBuffer commandBuffer) const;
    void End(vk::CommandBuffer commandBuffer) const;
    void Resize(vk::Extent2D extent);

private:
    std::unique_ptr<RenderPass> m_RenderPass;
    std::unique_ptr<Texture> m_ColorTexture, m_DepthTexture;
    vk::Framebuffer m_Framebuffer;

    vk::Format m_Format;
    vk::Extent2D m_Extent;
    vk::Bool32 m_RenderDepth, m_RenderColor, m_ImGuiReadable;
};
