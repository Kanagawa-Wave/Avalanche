#include "RenderTarget.h"

#include "Context.h"

RenderTarget::RenderTarget(vk::Format format, vk::Extent2D extent, vk::Bool32 renderDepth)
    : m_Extent(extent)
{
    const auto& device = Context::Instance().GetDevice();

    m_Image = std::make_unique<Image>(format, extent, vk::ImageUsageFlagBits::eColorAttachment |
                                      vk::ImageUsageFlagBits::eSampled);

    RenderPassCreateInfo renderPassInfo;
    renderPassInfo.setLoadOp(vk::AttachmentLoadOp::eClear)
                  .setStoreOp(vk::AttachmentStoreOp::eStore)
                  .setColorAttachmentFormat(format)
                  .setEnableDepthAttachment(renderDepth)
                  .setDepthAttachmentFormat(vk::Format::eD32Sfloat);
    m_RenderPass = std::make_unique<RenderPass>(renderPassInfo);

    vk::FramebufferCreateInfo framebufferInfo;
    std::array attachments = {m_Image->GetView()};

    framebufferInfo.setAttachments(attachments)
                   .setWidth(extent.width)
                   .setHeight(extent.height)
                   .setRenderPass(m_RenderPass->GetRenderPass())
                   .setLayers(1);
    m_Framebuffer = device.createFramebuffer(framebufferInfo);
}

RenderTarget::~RenderTarget()
{
    const auto& device = Context::Instance().GetDevice();

    device.waitIdle();
    device.destroyFramebuffer(m_Framebuffer);
}

void RenderTarget::Begin(vk::CommandBuffer commandBuffer) const
{
    vk::RenderPassBeginInfo renderPassBegin;
    vk::Rect2D area;
    vk::ClearValue color, depth;

    area.setOffset({0, 0})
        .setExtent(m_Extent);
    color.setColor({0.1f, 0.1f, 0.1f, 1.0f});
    depth.setDepthStencil(1.0f);
    std::array clearValues = {color, depth};

    renderPassBegin.setRenderPass(m_RenderPass->GetRenderPass())
                   .setFramebuffer(m_Framebuffer)
                   .setClearValues(clearValues)
                   .setRenderArea(area);
    
    commandBuffer.beginRenderPass(renderPassBegin, vk::SubpassContents::eInline);
}

void RenderTarget::End(vk::CommandBuffer commandBuffer) const
{
    commandBuffer.endRenderPass();
}
