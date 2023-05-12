#pragma once

#include <vulkan/vulkan.hpp>

#include "Buffers.h"
#include "DescriptorSet.h"
#include "Shader.h"

struct PipelineCreateInfo
{
private:
    std::string VertexShader;
    std::string FragmentShader;
    vk::Extent2D Extent;
    uint32_t PushConstantSize = 0;
    VertexInputInfo VertexInputInfo;
    vk::RenderPass RenderPass;
    vk::ArrayProxy<vk::DescriptorSetLayout> DescriptorSetLayouts;

public:
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

    PipelineCreateInfo& setExtent(vk::Extent2D extent)
    {
        Extent = extent;
        return *this;
    }

    PipelineCreateInfo& setPushConstantSize(uint32_t size)
    {
        PushConstantSize = size;
        return *this;
    }

    PipelineCreateInfo& setRenderPass(vk::RenderPass renderPass)
    {
        RenderPass = renderPass;
        return *this;
    }

    PipelineCreateInfo& setDescriptorSetLayouts(vk::ArrayProxy<vk::DescriptorSetLayout> layouts)
    {
        DescriptorSetLayouts = layouts;
        return *this;
    }
};

class Pipeline
{
public:
    Pipeline(const std::string& vertPath, const std::string& fragPath, vk::Extent2D extent, uint32_t pushConstantSize,
             const VertexInputInfo& vertexInputInfo, vk::RenderPass renderPass);
    ~Pipeline();

    vk::PipelineLayout GetLayout() const { return m_Layout; }
    vk::Pipeline GetPipeline() const { return m_Pipeline; }
    Shader& GetShader() const { return *m_Shader.get(); }

    void CreateLayout(uint32_t pushConstantSize);
    void CreatePipeline(uint32_t width, uint32_t height, const VertexInputInfo& vertexInputInfo,
                        vk::RenderPass renderPass);

    void UpdateUniformBuffer(Buffer* buffer, uint32_t binding);
    void Bind(vk::CommandBuffer commandBuffer) const;
    void BindDescriptorSet(vk::CommandBuffer commandBuffer) const;

private:
    void InitPipelineDescriptorSet();

    std::unique_ptr<Shader> m_Shader;
    vk::Pipeline m_Pipeline;
    vk::PipelineLayout m_Layout;

    std::unique_ptr<DescriptorSet> m_PipelineDescriptorSet;
};
