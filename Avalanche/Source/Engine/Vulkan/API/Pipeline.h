#pragma once

#include <vulkan/vulkan.hpp>

#include "Buffers.h"
#include "Shader.h"

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

    void SetUniformBuffer(Buffer& buffer, uint32_t binding);
    void Bind(vk::CommandBuffer commandBuffer) const;
    void BindBuffer(vk::CommandBuffer commandBuffer, const Buffer& buffer) const;

private:
    void InitDescriptors();

    std::unique_ptr<Shader> m_Shader;
    vk::Pipeline m_Pipeline;
    vk::PipelineLayout m_Layout;

    vk::DescriptorPool m_DescriptorPool;
    vk::DescriptorSetLayout m_GlobalSetLayout;
};
