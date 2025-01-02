#pragma once

#include <vulkan/vulkan.hpp>

#include "DescriptorSet.h"

struct ShaderDataLayout
{
	uint32_t Binding;
    vk::DescriptorType Type;
    uint32_t Count;
    vk::ShaderStageFlags Stage;
    uint32_t Size;

    ShaderDataLayout(uint32_t binding, vk::DescriptorType type, uint32_t count, vk::ShaderStageFlags stage, uint32_t size)
	    : Binding(binding), Type(type), Count(count), Stage(stage), Size(size)
    {}
};

struct ShaderCreateInfo
{
	std::string VertPath, FragPath;
    vk::ArrayProxy<ShaderDataLayout> GlobalSetLayout;

    ShaderCreateInfo& setShaderPaths(const std::string& vertPath, const std::string& fragPath)
    {
	    VertPath = vertPath, FragPath = fragPath;
        return *this;
    }

    ShaderCreateInfo& setStaticSetLayout(const vk::ArrayProxy<ShaderDataLayout>& layout)
    {
	    GlobalSetLayout = layout;
        return *this;
    }
};

class Shader
{
public:
    Shader(const ShaderCreateInfo& info);
    ~Shader();

    const std::vector<vk::PipelineShaderStageCreateInfo>& GetStageInfo() { return m_Stages; }

    void SetBufferData(uint32_t binding, const void* data) const;

private:
	void SetGlobalDescriptorLayout(const vk::ArrayProxy<ShaderDataLayout>& layout);

    static std::vector<char> LoadSPVFromFile(const std::string& path);
    void InitPipelineShaderStageCreateInfo();
    
private:
    vk::ShaderModule m_VertexShader, m_FragmentShader;
    std::vector<vk::PipelineShaderStageCreateInfo> m_Stages;
    std::unique_ptr<DescriptorSet> m_GlobalDescriptorSet;
    std::vector<std::unique_ptr<Buffer>> m_UniformBuffers;

    friend class Pipeline;
};
