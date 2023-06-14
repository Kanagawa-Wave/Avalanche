#include "RenderTarget.h"

#include "Context.h"

RenderTarget::RenderTarget(vk::Format format, vk::Extent2D extent, vk::Bool32 renderDepth)
    : m_Format(format), m_Extent(extent), m_RenderDepth(renderDepth)
{
    const auto& device = Context::Instance().GetDevice();

    m_RenderTexture = std::make_unique<Texture>(format, extent, vk::ImageUsageFlagBits::eColorAttachment |
                                                vk::ImageUsageFlagBits::eSampled);

    if (renderDepth)
    {
        m_DepthTexture = std::make_unique<Texture>(vk::Format::eD32Sfloat, extent,
                                                   vk::ImageUsageFlagBits::eDepthStencilAttachment);
    }

    RenderPassCreateInfo renderPassInfo;
    renderPassInfo.setLoadOp(vk::AttachmentLoadOp::eClear)
                  .setStoreOp(vk::AttachmentStoreOp::eStore)
                  .setColorAttachmentFormat(format)
                  .setEnableDepthAttachment(renderDepth)
                  .setDepthAttachmentFormat(vk::Format::eD32Sfloat)
                  .setInitialLayout(vk::ImageLayout::eUndefined)
                  .setFinalLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
    m_RenderPass = std::make_unique<RenderPass>(renderPassInfo);

    vk::FramebufferCreateInfo framebufferInfo;
    std::vector attachments = {m_RenderTexture->GetView()};

    if (renderDepth)
        attachments.emplace_back(m_DepthTexture->GetView());

    framebufferInfo.setAttachments(attachments)
                   .setWidth(extent.width)
                   .setHeight(extent.height)
                   .setRenderPass(m_RenderPass->GetRenderPass())
                   .setLayers(1);
    m_Framebuffer = device.createFramebuffer(framebufferInfo);

    m_RenderTexture->RegisterForImGui();
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

void RenderTarget::Resize(vk::Extent2D extent)
{
    if (m_Extent == extent)
        return;
    
    const auto& device = Context::Instance().GetDevice();
    
    m_Extent = extent;
    m_RenderTexture.reset();
    m_DepthTexture.reset();
    device.destroyFramebuffer(m_Framebuffer);

    m_RenderTexture = std::make_unique<Texture>(m_Format, m_Extent, vk::ImageUsageFlagBits::eColorAttachment |
                                                vk::ImageUsageFlagBits::eSampled);

    if (m_RenderDepth)
    {
        m_DepthTexture = std::make_unique<Texture>(vk::Format::eD32Sfloat, extent,
                                                   vk::ImageUsageFlagBits::eDepthStencilAttachment);
    }

    vk::FramebufferCreateInfo framebufferInfo;
    std::vector attachments = {m_RenderTexture->GetView()};

    if (m_RenderDepth)
        attachments.emplace_back(m_DepthTexture->GetView());

    framebufferInfo.setAttachments(attachments)
                   .setWidth(extent.width)
                   .setHeight(extent.height)
                   .setRenderPass(m_RenderPass->GetRenderPass())
                   .setLayers(1);
    m_Framebuffer = device.createFramebuffer(framebufferInfo);

    m_RenderTexture->RegisterForImGui();
}
