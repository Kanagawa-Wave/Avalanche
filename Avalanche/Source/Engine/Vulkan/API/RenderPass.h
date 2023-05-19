#pragma once

#include <vulkan/vulkan.hpp>

struct RenderPassCreateInfo
{
    vk::Format ColorFormat = vk::Format::eUndefined, DepthFormat = vk::Format::eUndefined;
    vk::Bool32 EnableDepthAttachment = false;
    vk::AttachmentLoadOp LoadOp = vk::AttachmentLoadOp::eDontCare;
    vk::AttachmentStoreOp StoreOp = vk::AttachmentStoreOp::eDontCare;

    RenderPassCreateInfo& setColorAttachmentFormat(vk::Format format)
    {
        ColorFormat = format;
        return *this;
    }

    RenderPassCreateInfo& setDepthAttachmentFormat(vk::Format format)
    {
        DepthFormat = format;
        return *this;
    }

    RenderPassCreateInfo& setEnableDepthAttachment(vk::Bool32 enableDepthAttachment)
    {
        EnableDepthAttachment = enableDepthAttachment;
        return *this;
    }

    RenderPassCreateInfo& setLoadOp(vk::AttachmentLoadOp loadOp)
    {
        LoadOp = loadOp;
        return *this;
    }

    RenderPassCreateInfo& setStoreOp(vk::AttachmentStoreOp storeOp)
    {
        StoreOp = storeOp;
        return *this;
    }
};

class RenderPass
{
public:
    RenderPass(const RenderPassCreateInfo& info);
    ~RenderPass();

    const vk::RenderPass& GetRenderPass() const { return m_RenderPass; }

private:
    vk::RenderPass m_RenderPass;
};
