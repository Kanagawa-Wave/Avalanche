#pragma once

#include <vulkan/vulkan.hpp>

#include "Buffers.h"
#include "DescriptorSet.h"
#include "Shader.h"

struct PipelineCreateInfo
{
    std::string VertexShader;
    std::string FragmentShader;
    uint32_t PushConstantSize = 0;
    VertexInputInfo VertexInput;
    vk::RenderPass RenderPass;
    vk::ArrayProxy<vk::DescriptorSetLayout> DescriptorSetLayouts;

    PipelineCreateInfo& setVertexShader(const std::string& vertexShader)
    {
        VertexShader = vertexShader;
        return *this;
    }

    PipelineCreateInfo& setFragmentShader(const std::string& fragmentShader)
    {
        FragmentShader = fragmentShader;
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

    PipelineCreateInfo& setDescriptorSetLayouts(const vk::ArrayProxy<vk::DescriptorSetLayout>& layouts)
    {
        DescriptorSetLayouts = layouts;
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
    void BindDescriptorSet(vk::CommandBuffer commandBuffer, vk::DescriptorSet descriptorSet) const;

private:
    void CreateLayout(uint32_t pushConstantSize, const vk::ArrayProxy<vk::DescriptorSetLayout>& layouts);
    void CreatePipeline(const VertexInputInfo& vertexInputInfo,
                        vk::RenderPass renderPass);

    std::unique_ptr<Shader> m_Shader;
    vk::Pipeline m_Pipeline;
    vk::PipelineLayout m_Layout;
};
