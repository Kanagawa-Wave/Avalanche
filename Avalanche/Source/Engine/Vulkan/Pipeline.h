#pragma once

#include <vulkan/vulkan.hpp>

#include "Shader.h"

class Pipeline
{
public:
    Pipeline(const std::string& vertPath, const std::string& fragPath);
    ~Pipeline();

    vk::Pipeline GetPipeline() const { return m_Pipeline; }
    vk::RenderPass GetRenderPass() const { return m_RenderPass; }

    void CreateLayout();
    void CreatePipeline(uint32_t width, uint32_t height);
    void CreateRenderPass();

private:
    std::unique_ptr<Shader> m_Shader;
    vk::Pipeline m_Pipeline;
    vk::PipelineLayout m_Layout;
    vk::RenderPass m_RenderPass;
};
