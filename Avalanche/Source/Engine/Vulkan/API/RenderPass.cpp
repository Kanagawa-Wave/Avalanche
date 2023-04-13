#include "RenderPass.h"

#include "Engine/Vulkan/Context.h"

RenderPass::RenderPass(vk::Format colorFormat)
{
    auto& ctx = Context::Instance();

    vk::RenderPassCreateInfo renderPassInfo;
    vk::AttachmentDescription colorAttachment, depthAttachment;
    colorAttachment.setFormat(colorFormat)
                   .setInitialLayout(vk::ImageLayout::eUndefined)
                   .setFinalLayout(vk::ImageLayout::ePresentSrcKHR)
                   .setLoadOp(vk::AttachmentLoadOp::eClear)
                   .setStoreOp(vk::AttachmentStoreOp::eStore)
                   .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
                   .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
                   .setSamples(vk::SampleCountFlagBits::e1);
    depthAttachment.setFormat(vk::Format::eD32Sfloat)
                   .setInitialLayout(vk::ImageLayout::eUndefined)
                   .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
                   .setLoadOp(vk::AttachmentLoadOp::eClear)
                   .setStoreOp(vk::AttachmentStoreOp::eStore)
                   .setStencilLoadOp(vk::AttachmentLoadOp::eClear)
                   .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
                   .setSamples(vk::SampleCountFlagBits::e1);

    std::array attachments = {colorAttachment, depthAttachment};
    renderPassInfo.setAttachments(attachments);

    vk::AttachmentReference colorRef, depthRef;
    colorRef.setLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setAttachment(0);
    depthRef.setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
            .setAttachment(1);
    vk::SubpassDescription subpass;
    subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
           .setColorAttachments(colorRef)
           .setPDepthStencilAttachment(&depthRef);
    renderPassInfo.setSubpasses(subpass);

    vk::SubpassDependency colorDependency, depthDependency;
    colorDependency.setSrcSubpass(VK_SUBPASS_EXTERNAL)
                   .setDstSubpass(0)
                   .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
                   .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
                   .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    depthDependency.setSrcSubpass(VK_SUBPASS_EXTERNAL)
                   .setDstSubpass(0)
                   .setDstAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentWrite)
                   .setSrcStageMask(
                       vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests)
                   .setDstStageMask(
                       vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests);

    std::array dependencies = {colorDependency, depthDependency};
    renderPassInfo.setDependencies(dependencies);

    m_RenderPass = Context::Instance().GetDevice().createRenderPass(renderPassInfo);
}

RenderPass::~RenderPass()
{
    auto& device = Context::Instance().GetDevice();
    device.destroyRenderPass(m_RenderPass);
}
