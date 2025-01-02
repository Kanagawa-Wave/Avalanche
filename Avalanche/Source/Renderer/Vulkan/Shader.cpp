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

    SetStaticDescriptorLayout(info.StaticLayout);
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

void Shader::SetStaticDescriptorLayout(const vk::ArrayProxy<ShaderDataLayout>& layout)
{
    std::vector<vk::DescriptorSetLayoutBinding> descriptorLayout;
    for (const auto& element : layout)
    {
        descriptorLayout.emplace_back(element.Binding, element.Type, element.Count, element.Stage);
    }
    m_StaticSet = Context::Instance().GetDescriptorSetBuilder()->CreateDescriptorSet(0);

    for (const auto& element : layout)
    {
	    m_UniformBuffers.emplace_back(std::make_unique<Buffer>(vk::BufferUsageFlagBits::eUniformBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU, element.Size));
    }

    for (int binding = 0; binding < m_UniformBuffers.size(); binding++)
    {
	    m_StaticSet->AttachUniformBuffer(m_UniformBuffers[binding].get(), binding);
    }
}

void Shader::AttachTexture(const Texture* texture, uint32_t binding) const
{
    m_DynamicSet->AttachTexture(texture, binding);
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
