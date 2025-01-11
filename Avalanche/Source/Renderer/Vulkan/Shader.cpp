#include "Shader.h"

#include "Core/Log.h"
#include "Renderer/Context.h"

Shader::Shader(const ShaderCreateInfo& info)
{
    const auto vertSource = LoadSPVFromFile(info.VertPath);
    const auto fragSource = LoadSPVFromFile(info.FragPath);

    vk::ShaderModuleCreateInfo shaderInfo;
    shaderInfo.setPCode(reinterpret_cast<const uint32_t*>(vertSource.data()))
              .setCodeSize(vertSource.size());
    m_VertexShader = Context::Instance().GetDevice().createShaderModule(shaderInfo);

    shaderInfo.setPCode(reinterpret_cast<const uint32_t*>(fragSource.data()))
              .setCodeSize(fragSource.size());
    m_FragmentShader = Context::Instance().GetDevice().createShaderModule(shaderInfo);

    InitPipelineShaderStageCreateInfo();

    SetGlobalDescriptorLayout(info.GlobalSetLayout, info.GlobalSetLayoutType);
}

Shader::~Shader()
{
    Context::Instance().GetDevice().destroyShaderModule(m_VertexShader);
    Context::Instance().GetDevice().destroyShaderModule(m_FragmentShader);
}

void Shader::SetBufferData(uint32_t binding, const void* data) const
{
    m_UniformBuffers[binding]->SetData(data);
}

void Shader::SetGlobalDescriptorLayout(const vk::ArrayProxy<ShaderDataLayout>& layout, EDescriptorSetLayoutType layoutType)
{
    m_GlobalDescriptorSet = Context::Instance().GetDescriptorSetBuilder()->CreateDescriptorSet(layoutType);

    for (const auto& element : layout)
    {
	    m_UniformBuffers.emplace_back(std::make_unique<Buffer>(vk::BufferUsageFlagBits::eUniformBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU, element.Size));
    }

    for (int binding = 0; binding < m_UniformBuffers.size(); binding++)
    {
	    m_GlobalDescriptorSet->UpdateDescriptor(m_UniformBuffers[binding].get(), binding);
    }
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
