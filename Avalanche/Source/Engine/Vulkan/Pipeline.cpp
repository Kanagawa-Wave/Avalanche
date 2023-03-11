#include "Pipeline.h"

#include "Context.h"
#include "Engine/Log/Log.h"

Pipeline::Pipeline(const std::string& vertPath, const std::string& fragPath)
{
    m_Shader.reset(new Shader(vertPath, fragPath));
}

Pipeline::~Pipeline()
{
    Context::Instance().GetDevice().destroyRenderPass(m_RenderPass);
    Context::Instance().GetDevice().destroyPipelineLayout(m_Layout);
    Context::Instance().GetDevice().destroyPipeline(m_Pipeline);
}

void Pipeline::CreateLayout(uint32_t pushConstantSize)
{
    vk::PipelineLayoutCreateInfo layoutInfo;
    vk::PushConstantRange pushConstantRange;
    pushConstantRange.setOffset(0)
                     .setSize(pushConstantSize)
                     .setStageFlags(vk::ShaderStageFlagBits::eVertex);
    layoutInfo.setPushConstantRanges(pushConstantRange);

    m_Layout = Context::Instance().GetDevice().createPipelineLayout(layoutInfo);
}

void Pipeline::CreatePipeline(uint32_t width, uint32_t height, const VertexInputInfo& vertexInputInfo)
{
    vk::GraphicsPipelineCreateInfo pipelineInfo;

    vk::PipelineVertexInputStateCreateInfo vertexInput;
    vertexInput.setVertexBindingDescriptions(vertexInputInfo.BindingDescription)
               .setVertexAttributeDescriptions(vertexInputInfo.AttributeDescriptions);
    pipelineInfo.setPVertexInputState(&vertexInput);

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
    inputAssembly.setPrimitiveRestartEnable(false)
                 .setTopology(vk::PrimitiveTopology::eTriangleList);
    pipelineInfo.setPInputAssemblyState(&inputAssembly);

    pipelineInfo.setStages(m_Shader->GetStageInfo());

    vk::PipelineViewportStateCreateInfo viewportState;
    vk::Viewport viewport(0.f, (float)height, (float)width, -(float)height, 0.f, 1.f);
    vk::Rect2D rect(vk::Offset2D(0, 0), vk::Extent2D(width, height));
    viewportState.setViewports(viewport)
                 .setScissors(rect);
    pipelineInfo.setPViewportState(&viewportState);

    vk::PipelineRasterizationStateCreateInfo rasterizationState;
    rasterizationState.setRasterizerDiscardEnable(false)
                      .setCullMode(vk::CullModeFlagBits::eBack)
                      .setFrontFace(vk::FrontFace::eClockwise)
                      .setPolygonMode(vk::PolygonMode::eFill)
                      .setLineWidth(1.f);
    pipelineInfo.setPRasterizationState(&rasterizationState);

    vk::PipelineMultisampleStateCreateInfo multisampleInfo;
    multisampleInfo.setSampleShadingEnable(false)
                   .setRasterizationSamples(vk::SampleCountFlagBits::e1);
    pipelineInfo.setPMultisampleState(&multisampleInfo);

    vk::PipelineColorBlendAttachmentState attachment;
    attachment.setBlendEnable(false)
              .setColorWriteMask(
                  vk::ColorComponentFlagBits::eA |
                  vk::ColorComponentFlagBits::eB |
                  vk::ColorComponentFlagBits::eG |
                  vk::ColorComponentFlagBits::eR);
    vk::PipelineColorBlendStateCreateInfo colorBlend;
    colorBlend.setLogicOpEnable(false)
              .setAttachments(attachment);
    pipelineInfo.setPColorBlendState(&colorBlend);

    pipelineInfo.setRenderPass(m_RenderPass)
                .setLayout(m_Layout);

    auto result = Context::Instance().GetDevice().createGraphicsPipeline(nullptr, pipelineInfo);
    if (result.result != vk::Result::eSuccess)
    {
        ASSERT(0, "Failed to create pipeline")
    }

    m_Pipeline = result.value;
}

void Pipeline::CreateRenderPass()
{
    vk::RenderPassCreateInfo renderPassInfo;
    vk::AttachmentDescription attachment;
    attachment.setFormat(Context::Instance().GetSwapchain().GetFormat())
              .setInitialLayout(vk::ImageLayout::eUndefined)
              .setFinalLayout(vk::ImageLayout::ePresentSrcKHR)
              .setLoadOp(vk::AttachmentLoadOp::eClear)
              .setStoreOp(vk::AttachmentStoreOp::eStore)
              .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
              .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
              .setSamples(vk::SampleCountFlagBits::e1);
    renderPassInfo.setAttachments(attachment);

    vk::AttachmentReference attachmentRef;
    attachmentRef.setLayout(vk::ImageLayout::eColorAttachmentOptimal)
                 .setAttachment(0);
    vk::SubpassDescription subpass;
    subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
           .setColorAttachments(attachmentRef);
    renderPassInfo.setSubpasses(subpass);

    vk::SubpassDependency dependency;
    dependency.setSrcSubpass(VK_SUBPASS_EXTERNAL)
              .setDstSubpass(0)
              .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
              .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
              .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    renderPassInfo.setDependencies(dependency);

    m_RenderPass = Context::Instance().GetDevice().createRenderPass(renderPassInfo);
}
