#include "PipelineBuilder.h"

#include <spdlog/fmt/bundled/format.h>

#include "Engine/Log/Log.h"

vk::Pipeline PipelineBuilder::Build(vk::Device device, vk::RenderPass renderPass)
{
    vk::PipelineViewportStateCreateInfo pipelineViewportState;
    pipelineViewportState.setViewports(Viewport)
                         .setScissors(Scissor);

    vk::PipelineColorBlendStateCreateInfo pipelineColorBlend;
    pipelineColorBlend.setLogicOpEnable(false)
                      .setLogicOp(vk::LogicOp::eCopy)
                      .setAttachments(ColorBlendAttachment);

    vk::GraphicsPipelineCreateInfo pipelineInfo;
    pipelineInfo.setStages(ShaderStages)
                .setPVertexInputState(&VertexInputInfo)
                .setPInputAssemblyState(&InputAssembly)
                .setPViewportState(&pipelineViewportState)
                .setPRasterizationState(&Rasterizer)
                .setPMultisampleState(&Multisampling)
                .setPColorBlendState(&pipelineColorBlend)
                .setLayout(PipelineLayout)
                .setRenderPass(renderPass)
                .setSubpass(0)
                .setBasePipelineHandle(nullptr);

    vk::Result result;
    vk::Pipeline pipeline;
    std::tie(result, pipeline) = device.createGraphicsPipeline(nullptr, pipelineInfo);
    CHECK(result);

    return pipeline;
}
