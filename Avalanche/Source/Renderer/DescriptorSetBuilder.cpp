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

void DescriptorSetBuilder::SetLayout(uint32_t layoutID, const vk::ArrayProxy<vk::DescriptorSetLayoutBinding>& bindings)
{
    const auto& device = Context::Instance().GetDevice();

    ASSERT(layoutID < m_NumLayouts, "Layout ID out of range")
    
    vk::DescriptorSetLayoutCreateInfo layoutInfo;
    layoutInfo.setBindings(bindings);

    m_DescriptorSetLayouts[layoutID] = device.createDescriptorSetLayout(layoutInfo);
}

std::shared_ptr<DescriptorSet> DescriptorSetBuilder::CreateDescriptorSet(uint32_t layoutID)
{
    const auto& context = Context::Instance();
    
    return std::make_shared<DescriptorSet>(context.GetDescriptorPool(), m_DescriptorSetLayouts[layoutID]);
}

std::vector<vk::DescriptorSetLayout> DescriptorSetBuilder::GetDescriptorSetLayouts()
{
    return m_DescriptorSetLayouts;
}
