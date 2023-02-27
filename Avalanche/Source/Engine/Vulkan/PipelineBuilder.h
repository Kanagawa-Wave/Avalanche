#pragma once

#include "Types.h"

class PipelineBuilder
{
public:
	vk::Pipeline Build(vk::Device device, vk::RenderPass renderPass);

private:
	std::vector<vk::PipelineShaderStageCreateInfo> m_ShaderStages;
	vk::PipelineVertexInputStateCreateInfo m_VertexInputInfo;
	vk::PipelineInputAssemblyStateCreateInfo m_InputAssembly;
	vk::Viewport m_Viewport;
	vk::Rect2D m_Scissor;
	vk::PipelineRasterizationStateCreateInfo m_Rasterizer;
	vk::PipelineColorBlendStateCreateInfo m_ColorBlendAttachment;
	vk::PipelineMultisampleStateCreateInfo m_Multisampling;
	vk::PipelineLayout m_PipelineLayout;
};

