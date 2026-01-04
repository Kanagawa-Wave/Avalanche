#include "ShaderResourceLayout.h"

#include <ranges>

#include "Context.h"

ShaderResourceLayout::ShaderResourceLayout(std::span<const BindingInfo> bindings, uint32_t maxSet)
{
    auto device = Context::Instance().GetDevice();
    
    m_Layouts.resize(maxSet + 1);
    m_BindingCount = bindings.size();
    for (uint32_t i = 0; i < bindings.size(); ++i)
    {
        vk::DescriptorSetLayoutBinding binding;
        binding.setBinding(bindings[i].binding)
            .setDescriptorCount(bindings[i].count)
            .setDescriptorType(bindings[i].type)
            .setStageFlags(bindings[i].stages);
        m_Bindings[bindings[i].set].push_back(binding);
    }
    
    for (const auto& binding : m_Bindings)
    {
        vk::DescriptorSetLayoutCreateInfo layoutInfo;
        layoutInfo.setBindings(binding.second);
        m_Layouts[binding.first] = device.createDescriptorSetLayout(layoutInfo);
    }
    
}

ShaderResourceLayout::~ShaderResourceLayout()
{
    auto device = Context::Instance().GetDevice();
    for (auto layout : m_Layouts)
    {
        device.destroyDescriptorSetLayout(layout);
    }
}
