#include "Pipeline.h"

#include "Engine/Vulkan/Context.h"
#include "Engine/Core/Log.h"

Pipeline::Pipeline(const std::string& vertPath, const std::string& fragPath, vk::Extent2D extent,
                   uint32_t pushConstantSize, const VertexInputInfo& vertexInputInfo, vk::RenderPass renderPass)
{
    m_Shader.reset(new Shader(vertPath, fragPath));

    InitDescriptors();
    CreateLayout(pushConstantSize);
    CreatePipeline(extent.width, extent.height, vertexInputInfo, renderPass);
}

Pipeline::~Pipeline()
{
    auto& device = Context::Instance().GetDevice();

    device.destroyDescriptorSetLayout(m_GlobalSetLayout);
    device.destroyDescriptorPool(m_DescriptorPool);
    device.destroyPipelineLayout(m_Layout);
    device.destroyPipeline(m_Pipeline);
}

void Pipeline::CreateLayout(uint32_t pushConstantSize)
{
    vk::PipelineLayoutCreateInfo layoutInfo;
    vk::PushConstantRange pushConstantRange;
    pushConstantRange.setOffset(0)
                     .setSize(pushConstantSize)
                     .setStageFlags(vk::ShaderStageFlagBits::eVertex);
    layoutInfo.setPushConstantRanges(pushConstantRange);
    layoutInfo.setSetLayouts(m_GlobalSetLayout);

    m_Layout = Context::Instance().GetDevice().createPipelineLayout(layoutInfo);
}

void Pipeline::CreatePipeline(uint32_t width, uint32_t height, const VertexInputInfo& vertexInputInfo,
                              vk::RenderPass renderPass)
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

    vk::PipelineDepthStencilStateCreateInfo depthStencil;
    depthStencil.setDepthTestEnable(true)
                .setDepthWriteEnable(true)
                .setDepthCompareOp(vk::CompareOp::eAlways)
                .setDepthBoundsTestEnable(false)
                .setMinDepthBounds(0.0)
                .setMaxDepthBounds(1.0)
                .setStencilTestEnable(false);
    pipelineInfo.setPDepthStencilState(&depthStencil);

    pipelineInfo.setRenderPass(renderPass)
                .setLayout(m_Layout);

    auto result = Context::Instance().GetDevice().createGraphicsPipeline(nullptr, pipelineInfo);
    if (result.result != vk::Result::eSuccess)
    {
        ASSERT(0, "Failed to create pipeline")
    }

    m_Pipeline = result.value;
}

void Pipeline::SetUniformBuffer(Buffer& buffer, uint32_t binding)
{
    auto& device = Context::Instance().GetDevice();

    vk::DescriptorSetAllocateInfo allocateInfo;
    allocateInfo.setSetLayouts(m_GlobalSetLayout)
                .setDescriptorSetCount(1)
                .setDescriptorPool(m_DescriptorPool);

    const auto descriptorSet = device.allocateDescriptorSets(allocateInfo)[0];
    buffer.SetDescriptor(descriptorSet);

    vk::DescriptorBufferInfo bufferInfo;
    bufferInfo.setBuffer(buffer.GetBuffer())
              .setOffset(0)
              .setRange(buffer.GetSize());

    vk::WriteDescriptorSet setWrite;
    setWrite.setDstBinding(binding)
            .setDstSet(descriptorSet)
            .setDescriptorCount(1)
            .setBufferInfo(bufferInfo)
            .setDescriptorType(vk::DescriptorType::eUniformBuffer);

    device.updateDescriptorSets(setWrite, nullptr);
}

void Pipeline::Bind(vk::CommandBuffer commandBuffer) const
{
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_Pipeline);
}

void Pipeline::BindBuffer(vk::CommandBuffer commandBuffer, const Buffer& buffer) const
{
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_Layout, 0, buffer.GetDescriptor(), nullptr);
}

void Pipeline::InitDescriptors()
{
    auto& device = Context::Instance().GetDevice();

    vk::DescriptorSetLayoutBinding cameraBufferBinding;
    cameraBufferBinding.setBinding(0)
                       .setDescriptorCount(1)
                       .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                       .setStageFlags(vk::ShaderStageFlagBits::eVertex);

    vk::DescriptorSetLayoutCreateInfo setLayout;
    setLayout.setBindings(cameraBufferBinding);

    m_GlobalSetLayout = device.createDescriptorSetLayout(setLayout);

    vk::DescriptorPoolCreateInfo poolInfo;
    std::vector<vk::DescriptorPoolSize> poolSizes = {{vk::DescriptorType::eUniformBuffer, 1000}};
    poolInfo.setPoolSizes(poolSizes)
            .setMaxSets(1000);

    m_DescriptorPool = device.createDescriptorPool(poolInfo);
}
