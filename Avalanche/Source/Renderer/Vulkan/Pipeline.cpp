﻿#include "Pipeline.h"

#include "Core/Log.h"
#include "Renderer/Context.h"

Pipeline::Pipeline(const PipelineCreateInfo& pipelineCreateInfo)
{
	const Context& context = Context::Instance();
	m_Shader.reset(new Shader(pipelineCreateInfo.ShaderInfo));

	// TODO: Remove hard coded layout IDs
	auto layouts = context.GetDescriptorSetBuilder()->GetDescriptorSetLayouts(
	{
		pipelineCreateInfo.ShaderInfo.GlobalSetLayoutType,
		EDescriptorSetLayoutType::PerModelSet
	});
	CreateLayout(pipelineCreateInfo.PushConstantSize,layouts);
	CreatePipeline(pipelineCreateInfo);
}

Pipeline::~Pipeline()
{
	const auto& device = Context::Instance().GetDevice();

	device.waitIdle();

	device.destroyPipelineLayout(m_Layout);
	device.destroyPipeline(m_Pipeline);
}

void Pipeline::CreateLayout(uint32_t pushConstantSize, const std::vector<vk::DescriptorSetLayout>& layouts)
{

	vk::PipelineLayoutCreateInfo layoutInfo{};

	if (pushConstantSize != 0)
	{
		vk::PushConstantRange pushConstantRange;
		pushConstantRange.setOffset(0)
			.setSize(pushConstantSize)
			.setStageFlags(vk::ShaderStageFlagBits::eVertex);
		layoutInfo.setPushConstantRanges(pushConstantRange);
	}
	if (!layouts.empty())
		layoutInfo.setSetLayouts(layouts);

	m_Layout = Context::Instance().GetDevice().createPipelineLayout(layoutInfo);
}

void Pipeline::CreatePipeline(const PipelineCreateInfo& pipelineCreateInfo)
{
	vk::GraphicsPipelineCreateInfo pipelineInfo;

	vk::PipelineVertexInputStateCreateInfo vertexInput{};
	if (!pipelineCreateInfo.VertexInput.AttributeDescriptions.empty())
	{
		vertexInput.setVertexBindingDescriptions(pipelineCreateInfo.VertexInput.BindingDescription)
			.setVertexAttributeDescriptions(pipelineCreateInfo.VertexInput.AttributeDescriptions);
	}
	pipelineInfo.setPVertexInputState(&vertexInput);

	vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
	inputAssembly.setPrimitiveRestartEnable(false)
		.setTopology(vk::PrimitiveTopology::eTriangleList);
	pipelineInfo.setPInputAssemblyState(&inputAssembly);

	pipelineInfo.setStages(m_Shader->GetStageInfo());

	vk::PipelineViewportStateCreateInfo viewportState;
	viewportState.setViewportCount(1)
		.setScissorCount(1);
	pipelineInfo.setPViewportState(&viewportState);

	vk::PipelineDynamicStateCreateInfo dynamicState;
	std::vector<vk::DynamicState> states = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
	dynamicState.setDynamicStates(states);
	pipelineInfo.setPDynamicState(&dynamicState);

	vk::PipelineRasterizationStateCreateInfo rasterizationState;
	rasterizationState.setRasterizerDiscardEnable(false)
		.setCullMode(pipelineCreateInfo.CullMode)
		.setFrontFace(vk::FrontFace::eCounterClockwise)
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
		.setDepthCompareOp(vk::CompareOp::eLess)
		.setDepthBoundsTestEnable(false)
		.setMinDepthBounds(0.f)
		.setMaxDepthBounds(1.f)
		.setStencilTestEnable(false);
	pipelineInfo.setPDepthStencilState(&depthStencil);

	pipelineInfo.setRenderPass(pipelineCreateInfo.RenderPass)
		.setLayout(m_Layout);

	auto result = Context::Instance().GetDevice().createGraphicsPipeline(nullptr, pipelineInfo);
	if (result.result != vk::Result::eSuccess)
	{
		ASSERT(0, "Failed to create pipeline")
	}

	m_Pipeline = result.value;
}


void Pipeline::Bind(vk::CommandBuffer commandBuffer) const
{
	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_Pipeline);
	commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_Layout, 0, {m_Shader->m_GlobalDescriptorSet->GetDescriptorSet()}, nullptr);
}

void Pipeline::BindDescriptorSets(vk::CommandBuffer commandBuffer,
	const vk::ArrayProxy<vk::DescriptorSet>& descriptorSets,
	uint32_t firstSet) const
{
	commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_Layout, firstSet,
		descriptorSets,
		nullptr);
}

void Pipeline::SetShaderBufferData(uint32_t binding, const void* data) const
{
	m_Shader->SetBufferData(binding, data);
}
