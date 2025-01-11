#pragma once

#include <vulkan/vulkan.hpp>

#include "Buffers.h"
#include "DescriptorSet.h"
#include "Shader.h"

struct PipelineCreateInfo
{
    ShaderCreateInfo ShaderInfo;
    uint32_t PushConstantSize = 0;
    VertexInputInfo VertexInput;
    vk::RenderPass RenderPass;

    PipelineCreateInfo& setVertexShader(const std::string& vertexShader)
    {
        ShaderInfo.VertPath = vertexShader;
        return *this;
    }

    PipelineCreateInfo& setFragmentShader(const std::string& fragmentShader)
    {
        ShaderInfo.FragPath = fragmentShader;
        return *this;
    }

	PipelineCreateInfo& setGlobalSetLayout(const vk::ArrayProxy<ShaderDataLayout>& layout, EDescriptorSetLayoutType type)
    {
	    ShaderInfo.setGlobalSetLayout(layout);
        ShaderInfo.setGlobalSetLayoutType(type);
        return *this;
    }

    PipelineCreateInfo& setPushConstantSize(uint32_t size)
    {
        PushConstantSize = size;
        return *this;
    }

    PipelineCreateInfo& setVertexInputInfo(const VertexInputInfo& vertexInput)
    {
        VertexInput = vertexInput;
        return *this;
    }

    PipelineCreateInfo& setRenderPass(vk::RenderPass renderPass)
    {
        RenderPass = renderPass;
        return *this;
    }
};

class Pipeline
{
public:
    Pipeline(const PipelineCreateInfo& pipelineCreateInfo);
    ~Pipeline();

    vk::PipelineLayout GetLayout() const { return m_Layout; }
    vk::Pipeline GetPipeline() const { return m_Pipeline; }
    Shader* GetShader() const { return m_Shader.get(); }

    void Bind(vk::CommandBuffer commandBuffer) const;
    void BindDescriptorSets(vk::CommandBuffer commandBuffer,
                            const vk::ArrayProxy<vk::DescriptorSet>& descriptorSets,
                            uint32_t firstSet = 0) const;


	void SetShaderBufferData(uint32_t binding, const void* data) const;

private:
    void CreateLayout(uint32_t pushConstantSize, const std::vector<vk::DescriptorSetLayout>& layouts);
    void CreatePipeline(const VertexInputInfo& vertexInputInfo,
                        vk::RenderPass renderPass);

    std::unique_ptr<Shader> m_Shader;
    vk::Pipeline m_Pipeline;
    vk::PipelineLayout m_Layout;
};
