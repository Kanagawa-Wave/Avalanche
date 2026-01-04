#include "Shader.h"

#include "Core/Log.h"
#include "Renderer/Context.h"

#include <spirv_cross.hpp>

Shader::Shader(const ShaderCreateInfo& info)
{
    const auto vertSource = LoadSPVFromFile(info.VertPath);
    
    vk::ShaderModuleCreateInfo shaderInfo;
    shaderInfo.setPCode(vertSource.data())
              .setCodeSize(vertSource.size() * sizeof(uint32_t));
    m_VertexShader = Context::Instance().GetDevice().createShaderModule(shaderInfo);


    std::vector<BindingInfo> bindings;
    GetShaderResourcesLayout(vertSource, vk::ShaderStageFlagBits::eVertex, bindings);

    if (!info.FragPath.empty())
    {
        const auto fragSource = LoadSPVFromFile(info.FragPath);
        shaderInfo.setPCode(fragSource.data())
                  .setCodeSize(fragSource.size() * sizeof(uint32_t));
        m_FragmentShader = Context::Instance().GetDevice().createShaderModule(shaderInfo);

        GetShaderResourcesLayout(fragSource, vk::ShaderStageFlagBits::eFragment, bindings);

        m_HasPixelShader = true;
    }

    m_ResourceLayout = std::make_unique<ShaderResourceLayout>(bindings, m_MaxSet);

    InitPipelineShaderStageCreateInfo();
}

Shader::~Shader()
{
    Context::Instance().GetDevice().destroyShaderModule(m_VertexShader);
    Context::Instance().GetDevice().destroyShaderModule(m_FragmentShader);
}

std::vector<uint32_t> Shader::LoadSPVFromFile(const std::string& path)
{
    std::ifstream file(path, std::ios::ate | std::ios::binary);

    ASSERT(file.is_open(), "Failed to open shader file at {}", path)

    const long long byteSize = file.tellg();
    ASSERT(byteSize % 4 == 0, "SPIR-V file size must be multiple of 4 bytes")
    std::vector<uint32_t> buffer(static_cast<size_t>(byteSize) / 4);

    file.seekg(0);
    file.read(reinterpret_cast<char*>(buffer.data()), byteSize);
    file.close();

    return buffer;
}

void Shader::InitPipelineShaderStageCreateInfo()
{
    m_Stages.resize(1);
    m_Stages[0].setStage(vk::ShaderStageFlagBits::eVertex)
               .setModule(m_VertexShader)
               .setPName("main");
    if (m_HasPixelShader)
    {
        m_Stages.resize(2);
        m_Stages[1].setStage(vk::ShaderStageFlagBits::eFragment)
                   .setModule(m_FragmentShader)
                   .setPName("main");
    }
}

void Shader::GetShaderResourcesLayout(const std::vector<uint32_t>& source, vk::ShaderStageFlagBits stage,
    std::vector<BindingInfo>& outBindings)
{
    spirv_cross::Compiler comp(source);
    spirv_cross::ShaderResources resources = comp.get_shader_resources();

    // Uniform Buffers
    for (auto& r : resources.uniform_buffers) {
        uint32_t binding = comp.get_decoration(r.id, spv::DecorationBinding);
        uint32_t set = comp.get_decoration(r.id, spv::DecorationDescriptorSet);
        outBindings.push_back({
            .set = set,
            .binding = binding,
            .type = vk::DescriptorType::eUniformBuffer,
            .stages = stage,
            .count = 1
        });
        m_MaxSet = std::max(m_MaxSet, set);
    }

    // Combined Image Samplers
    for (auto& r : resources.sampled_images) {
        uint32_t binding = comp.get_decoration(r.id, spv::DecorationBinding);
        uint32_t set = comp.get_decoration(r.id, spv::DecorationDescriptorSet);
        outBindings.push_back({
            .set = set,
            .binding = binding,
            .type = vk::DescriptorType::eCombinedImageSampler,
            .stages = stage,
            .count = 1
        });
        m_MaxSet = std::max(m_MaxSet, set);
    }
}
