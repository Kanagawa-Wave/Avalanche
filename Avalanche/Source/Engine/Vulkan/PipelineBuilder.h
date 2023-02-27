#pragma once

#include "Types.h"

struct PipelineBuilder
{
	vk::Pipeline Build(vk::Device device, vk::RenderPass renderPass);
	
	std::vector<vk::PipelineShaderStageCreateInfo> ShaderStages;
	vk::PipelineVertexInputStateCreateInfo VertexInputInfo;
	vk::PipelineInputAssemblyStateCreateInfo InputAssembly;
	vk::Viewport Viewport;
	vk::Rect2D Scissor;
	vk::PipelineRasterizationStateCreateInfo Rasterizer;
	vk::PipelineColorBlendAttachmentState ColorBlendAttachment;
	vk::PipelineMultisampleStateCreateInfo Multisampling;
	vk::PipelineLayout PipelineLayout;
};

