#include "RenderPass.h"

#include "Renderer/Context.h"

RenderPass::RenderPass(const RenderPassCreateInfo& info)
{
    const auto& device = Context::Instance().GetDevice();

    vk::RenderPassCreateInfo renderPassInfo;
    vk::AttachmentDescription colorAttachment;
    std::vector<vk::AttachmentDescription> attachments;
    colorAttachment.setFormat(info.ColorFormat)
                   .setInitialLayout(info.InitialLayout)
                   .setFinalLayout(info.FinalLayout)
                   .setLoadOp(info.LoadOp)
                   .setStoreOp(info.StoreOp)
                   .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
                   .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
                   .setSamples(vk::SampleCountFlagBits::e1);
    attachments.push_back(colorAttachment);

    if (info.EnableDepthAttachment)
    {
        vk::AttachmentDescription depthAttachment;
        depthAttachment.setFormat(info.DepthFormat)
                       .setInitialLayout(vk::ImageLayout::eUndefined)
                       .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
                       .setLoadOp(vk::AttachmentLoadOp::eClear)
                       .setStoreOp(vk::AttachmentStoreOp::eStore)
                       .setStencilLoadOp(vk::AttachmentLoadOp::eClear)
                       .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
                       .setSamples(vk::SampleCountFlagBits::e1);
        attachments.push_back(depthAttachment);
    }

    renderPassInfo.setAttachments(attachments);

    vk::SubpassDescription subpass;
    vk::AttachmentReference colorRef;
    colorRef.setLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setAttachment(0);
    subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
           .setColorAttachments(colorRef);
    if (info.EnableDepthAttachment)
    {
        vk::AttachmentReference depthRef;
        depthRef.setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
                .setAttachment(1);
        subpass.setPDepthStencilAttachment(&depthRef);
    }
    renderPassInfo.setSubpasses(subpass);

    vk::SubpassDependency colorDependency;
    std::vector<vk::SubpassDependency> dependencies;
    colorDependency.setSrcSubpass(VK_SUBPASS_EXTERNAL)
                   .setDstSubpass(0)
                   .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
                   .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
                   .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    dependencies.push_back(colorDependency);

    if (info.EnableDepthAttachment)
    {
        vk::SubpassDependency depthDependency;
        depthDependency.setSrcSubpass(VK_SUBPASS_EXTERNAL)
                       .setDstSubpass(0)
                       .setDstAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentWrite)
                       .setSrcStageMask(
                           vk::PipelineStageFlagBits::eEarlyFragmentTests |
                           vk::PipelineStageFlagBits::eLateFragmentTests)
                       .setDstStageMask(
                           vk::PipelineStageFlagBits::eEarlyFragmentTests |
                           vk::PipelineStageFlagBits::eLateFragmentTests);
        dependencies.push_back(depthDependency);
    }

    renderPassInfo.setDependencies(dependencies);

    m_RenderPass = device.createRenderPass(renderPassInfo);
}

RenderPass::~RenderPass()
{
    const auto& device = Context::Instance().GetDevice();

    device.waitIdle();
    device.destroyRenderPass(m_RenderPass);
}
