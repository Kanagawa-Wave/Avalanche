#pragma once

#include <vulkan/vulkan.hpp>

class Shader final
{
public:
    Shader(const std::string& vertPath, const std::string& fragPath);
    ~Shader();

    const std::vector<vk::PipelineShaderStageCreateInfo>& GetStageInfo() { return m_Stages; }

private:
    static std::vector<char> LoadSPVFromFile(const std::string& path);
    void InitPipelineShaderStageCreateInfo();
    
private:
    vk::ShaderModule m_VertexShader, m_FragmentShader;
    std::vector<vk::PipelineShaderStageCreateInfo> m_Stages;
};
