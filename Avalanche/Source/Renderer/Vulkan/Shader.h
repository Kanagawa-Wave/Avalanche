#pragma once

#include <vulkan/vulkan.hpp>

#include "DescriptorSet.h"
#include "Renderer/ShaderResourceLayout.h"

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

    ShaderCreateInfo& setShaderPaths(const std::string& vertPath, const std::string& fragPath)
    {
	    VertPath = vertPath;
        FragPath = fragPath;
        return *this;
    }
};

class Shader
{
public:
    Shader(const ShaderCreateInfo& info);
    ~Shader();

    const ShaderResourceLayout* GetShaderResourceLayout() const { return m_ResourceLayout.get(); }
    const std::vector<vk::PipelineShaderStageCreateInfo>& GetStageInfo() { return m_Stages; }

private:
    static std::vector<uint32_t> LoadSPVFromFile(const std::string& path);
    void InitPipelineShaderStageCreateInfo();
    void GetShaderResourcesLayout(const std::vector<uint32_t>& source, vk::ShaderStageFlagBits stage, std::vector<BindingInfo>& outBindings);
    
    vk::ShaderModule m_VertexShader, m_FragmentShader;
    std::vector<vk::PipelineShaderStageCreateInfo> m_Stages;
    bool m_HasPixelShader = false;
    uint32_t m_MaxSet = 0;

    std::unique_ptr<ShaderResourceLayout> m_ResourceLayout;
};
