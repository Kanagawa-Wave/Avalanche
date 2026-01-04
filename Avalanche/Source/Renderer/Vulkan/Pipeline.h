#pragma once

#include <vulkan/vulkan.hpp>

#include "Buffers.h"
#include "Shader.h"
#include "DescriptorSetWriter.h"
#include "Renderer/ShaderResourceLayout.h"

struct PipelineCreateInfo
{
    ShaderCreateInfo ShaderInfo;
    uint32_t PushConstantSize = 0;
    VertexInputInfo VertexInput;
    vk::RenderPass RenderPass;
    vk::CullModeFlags CullMode;

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

    PipelineCreateInfo& setCullMode(vk::CullModeFlags cullMode)
    {
        CullMode = cullMode;
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

private:
    void CreateLayout(uint32_t pushConstantSize, const std::vector<vk::DescriptorSetLayout>& layouts);
    void CreatePipeline(const PipelineCreateInfo& pipelineCreateInfo);

    std::unique_ptr<Shader> m_Shader;
    vk::Pipeline m_Pipeline;
    vk::PipelineLayout m_Layout;
};
