#pragma once

#include "Vulkan/DescriptorSet.h"

class DescriptorSetBuilder
{
public:
    DescriptorSetBuilder(uint32_t numLayouts);
    ~DescriptorSetBuilder();

    void SetLayout(uint32_t layoutID, const vk::ArrayProxy<vk::DescriptorSetLayoutBinding>& bindings);
    std::unique_ptr<DescriptorSet> CreateDescriptorSet(uint32_t layoutID);
    std::vector<vk::DescriptorSetLayout> GetDescriptorSetLayouts();

private:
    uint32_t m_NumLayouts = 0;
    std::vector<vk::DescriptorSetLayout> m_DescriptorSetLayouts = {};
};
