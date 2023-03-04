#pragma once

#include <vulkan/vulkan.hpp>

class Pipeline
{
public:
    Pipeline(vk::Device device, vk::RenderPass renderPass, vk::Extent2D extent, const std::string& vsh, const std::string& fsh);

    vk::Pipeline GetPipeline() const { return m_Pipeline; }
    
    operator vk::Pipeline() const { return m_Pipeline; }

private:
    enum class ShaderType
    {
        eVertex,
        eFragment,
        eCompute,
    };

    static void LoadShaderModule(vk::Device device, std::string path, ShaderType type, vk::ShaderModule& shaderModule);
    
    vk::PipelineLayout m_PipelineLayout;
    vk::Pipeline m_Pipeline;
};
