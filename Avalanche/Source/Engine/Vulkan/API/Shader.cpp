#include "Shader.h"

#include "Engine/Vulkan/Context.h"
#include "Engine/Core/Log.h"

Shader::Shader(const std::string& vertPath, const std::string& fragPath)
{
    const auto vertSource = LoadSPVFromFile(vertPath);
    const auto fragSource = LoadSPVFromFile(fragPath);

    vk::ShaderModuleCreateInfo shaderInfo;
    shaderInfo.setPCode(reinterpret_cast<const uint32_t*>(vertSource.data()))
              .setCodeSize(vertSource.size());
    m_VertexShader = Context::Instance().GetDevice().createShaderModule(shaderInfo);

    shaderInfo.setPCode(reinterpret_cast<const uint32_t*>(fragSource.data()))
              .setCodeSize(fragSource.size());
    m_FragmentShader = Context::Instance().GetDevice().createShaderModule(shaderInfo);

    InitPipelineShaderStageCreateInfo();
}

Shader::~Shader()
{
    Context::Instance().GetDevice().destroyShaderModule(m_VertexShader);
    Context::Instance().GetDevice().destroyShaderModule(m_FragmentShader);
}

std::vector<char> Shader::LoadSPVFromFile(const std::string& path)
{
    std::ifstream file(path, std::ios::ate | std::ios::binary);

    ASSERT(file.is_open(), "Failed to open shader file at {}", path)

    const long long fileSize = file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

void Shader::InitPipelineShaderStageCreateInfo()
{
    m_Stages.resize(2);
    m_Stages[0].setStage(vk::ShaderStageFlagBits::eVertex)
               .setModule(m_VertexShader)
               .setPName("main");
    m_Stages[1].setStage(vk::ShaderStageFlagBits::eFragment)
               .setModule(m_FragmentShader)
               .setPName("main");
}
