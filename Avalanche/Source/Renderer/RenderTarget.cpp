#include "RenderTarget.h"

#include "Context.h"

RenderTarget::RenderTarget(const RenderTargetCreateInfo& info)
    : m_Format(info.ColorFormat), m_Extent(info.Extent), m_RenderDepth(info.RenderDepth), m_RenderColor(info.RenderColor), m_ImGuiReadable(info.ImGuiReadable)
{
    const auto& device = Context::Instance().GetDevice();

    RenderPassCreateInfo renderPassInfo;
   
    std::vector<vk::ImageView> attachments;
    
    renderPassInfo.setLoadOp(vk::AttachmentLoadOp::eClear)
                  .setStoreOp(vk::AttachmentStoreOp::eStore)
                  .setColorAttachmentFormat(info.ColorFormat)
                  .setEnableDepthAttachment(info.RenderDepth)
                  .setDepthAttachmentFormat(vk::Format::eD32Sfloat)
                  .setInitialLayout(vk::ImageLayout::eUndefined)
                  .setFinalLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

    if (info.RenderColor)
    {
        m_ColorTexture = std::make_unique<Texture>(info.ColorFormat, info.Extent, vk::ImageUsageFlagBits::eColorAttachment |
                                                    vk::ImageUsageFlagBits::eSampled);
        attachments.emplace_back(m_ColorTexture->GetView());
    }

    if (info.RenderDepth)
    {
        m_DepthTexture = std::make_unique<Texture>(vk::Format::eD32Sfloat, info.Extent,
                                                   vk::ImageUsageFlagBits::eDepthStencilAttachment);
        attachments.emplace_back(m_DepthTexture->GetView());
    }

    m_RenderPass = std::make_unique<RenderPass>(renderPassInfo);

    vk::FramebufferCreateInfo framebufferInfo;
    framebufferInfo.setAttachments(attachments)
                   .setWidth(info.Extent.width)
                   .setHeight(info.Extent.height)
                   .setRenderPass(m_RenderPass->GetRenderPass())
                   .setLayers(1);
    m_Framebuffer = device.createFramebuffer(framebufferInfo);

    if (info.ImGuiReadable)
    {
        m_ColorTexture->RegisterForImGui();
    }
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
    std::vector<vk::ClearValue> clearValues = {};
    if (m_RenderColor)
        clearValues.emplace_back(color);
    if (m_RenderDepth)
        clearValues.emplace_back(depth);

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

    if (extent.width <= 0 || extent.height <= 0 || extent.width == UINT_MAX || extent.height == UINT_MAX)
        return;
    
    const auto& device = Context::Instance().GetDevice();
    
    m_Extent = extent;
    m_ColorTexture.reset();
    m_DepthTexture.reset();
    device.destroyFramebuffer(m_Framebuffer);

    std::vector<vk::ImageView> attachments;
    if (m_RenderColor)
    {
        m_ColorTexture = std::make_unique<Texture>(m_Format, extent, vk::ImageUsageFlagBits::eColorAttachment |
                                                    vk::ImageUsageFlagBits::eSampled);
        attachments.emplace_back(m_ColorTexture->GetView());
    }

    if (m_RenderDepth)
    {
        m_DepthTexture = std::make_unique<Texture>(vk::Format::eD32Sfloat, extent,
                                                   vk::ImageUsageFlagBits::eDepthStencilAttachment);
        attachments.emplace_back(m_DepthTexture->GetView());
    }

    vk::FramebufferCreateInfo framebufferInfo;

    framebufferInfo.setAttachments(attachments)
                   .setWidth(extent.width)
                   .setHeight(extent.height)
                   .setRenderPass(m_RenderPass->GetRenderPass())
                   .setLayers(1);
    m_Framebuffer = device.createFramebuffer(framebufferInfo);

    if (m_ImGuiReadable)
    {
        m_ColorTexture->RegisterForImGui();
    }
}
