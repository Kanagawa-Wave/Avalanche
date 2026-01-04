#pragma once
#include <ranges>
#include <vulkan/vulkan.hpp>
#include <unordered_map>

struct BindingInfo {
    uint32_t set, binding;
    vk::DescriptorType     type;
    vk::ShaderStageFlags   stages;
    uint32_t               count = 1;     // e.g. array of textures
};

class ShaderResourceLayout {
public:
    ShaderResourceLayout(std::span<const BindingInfo> bindings, uint32_t maxSet);
    ~ShaderResourceLayout();
    const std::vector<vk::DescriptorSetLayout>& GetLayouts() const { return m_Layouts; }
    vk::DescriptorSetLayout GetLayout(uint32_t set) const { return m_Layouts[set]; }
    uint32_t GetBindingCount() const { return m_BindingCount; }
private:
    uint32_t m_BindingCount;
    std::vector<vk::DescriptorSetLayout> m_Layouts;
    std::unordered_map<uint32_t, std::vector<vk::DescriptorSetLayoutBinding>> m_Bindings;
};
