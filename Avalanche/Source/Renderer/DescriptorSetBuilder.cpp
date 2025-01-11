#include "DescriptorSetBuilder.h"

#include "Context.h"

DescriptorSetBuilder::DescriptorSetBuilder(uint32_t numLayouts)
    : m_NumLayouts(numLayouts)
{
    m_DescriptorSetLayouts.resize(numLayouts);
}

DescriptorSetBuilder::~DescriptorSetBuilder()
{
    const auto& device = Context::Instance().GetDevice();

    for (uint32_t i = 0; i < m_NumLayouts; i++)
    {
        vkDestroyDescriptorSetLayout(device, m_DescriptorSetLayouts[i], nullptr);
    }
};

void DescriptorSetBuilder::SetLayout(EDescriptorSetLayoutType layoutID, const vk::ArrayProxy<vk::DescriptorSetLayoutBinding>& bindings)
{
    const auto& device = Context::Instance().GetDevice();

    ASSERT((int)layoutID < m_NumLayouts, "Layout ID out of range")
    
    vk::DescriptorSetLayoutCreateInfo layoutInfo;
    layoutInfo.setBindings(bindings);

    m_DescriptorSetLayouts[(int)layoutID] = device.createDescriptorSetLayout(layoutInfo);
}

std::unique_ptr<DescriptorSet> DescriptorSetBuilder::CreateDescriptorSet(EDescriptorSetLayoutType layoutID)
{
    const auto& context = Context::Instance();
    
    return std::make_unique<DescriptorSet>(context.GetDescriptorPool(), m_DescriptorSetLayouts[(int)layoutID]);
}

std::vector<vk::DescriptorSetLayout> DescriptorSetBuilder::GetDescriptorSetLayouts(std::vector<EDescriptorSetLayoutType> layoutIDs)
{
    std::vector<vk::DescriptorSetLayout> layouts;
    for (const auto& layoutID : layoutIDs)
    {
        layouts.push_back(m_DescriptorSetLayouts[(int)layoutID]);
    }
    return layouts;
}
