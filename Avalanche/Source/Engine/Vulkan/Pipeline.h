#pragma once

#include <vulkan/vulkan.hpp>

#include "Buffers.h"
#include "Shader.h"

class Pipeline
{
public:
    Pipeline(const std::string& vertPath, const std::string& fragPath);
    ~Pipeline();

    vk::PipelineLayout GetLayout() const { return m_Layout; }
    vk::Pipeline GetPipeline() const { return m_Pipeline; }
    vk::RenderPass GetRenderPass() const { return m_RenderPass; }
    Shader& GetShader() const { return *m_Shader.get(); }

    void CreateLayout(uint32_t pushConstantSize);
    void CreatePipeline(uint32_t width, uint32_t height, const VertexInputInfo& vertexInputInfo);
    void CreateRenderPass();

private:
    std::unique_ptr<Shader> m_Shader;
    vk::Pipeline m_Pipeline;
    vk::PipelineLayout m_Layout;
    vk::RenderPass m_RenderPass;
};
