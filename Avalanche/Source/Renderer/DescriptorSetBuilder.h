#pragma once

#include "Vulkan/DescriptorSet.h"

enum class EDescriptorSetLayoutType
{
    MainGlobalSet = 0,
    BillboardGlobalSet = 1,
    PerModelSet = 2,
};

class DescriptorSetBuilder
{
public:
    DescriptorSetBuilder(uint32_t numLayouts);
    ~DescriptorSetBuilder();

    void SetLayout(EDescriptorSetLayoutType layoutID, const vk::ArrayProxy<vk::DescriptorSetLayoutBinding>& bindings);
    std::unique_ptr<DescriptorSet> CreateDescriptorSet(EDescriptorSetLayoutType layoutID);
    std::vector<vk::DescriptorSetLayout> GetDescriptorSetLayouts(std::vector<EDescriptorSetLayoutType> layoutIDs);

private:
    uint32_t m_NumLayouts = 0;
    std::vector<vk::DescriptorSetLayout> m_DescriptorSetLayouts = {};
};
