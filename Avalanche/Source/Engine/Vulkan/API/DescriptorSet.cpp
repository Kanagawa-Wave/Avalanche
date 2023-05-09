#include "DescriptorSet.h"

#include "Engine/Vulkan/Context.h"

DescriptorSet::DescriptorSet(vk::DescriptorPool pool, const vk::ArrayProxy<vk::DescriptorSetLayoutBinding>& bindings)
{
    const auto& device = Context::Instance().GetDevice();

    vk::DescriptorSetLayoutCreateInfo layoutInfo;
    layoutInfo.setBindings(bindings);

    m_DescriptorSetLayout = device.createDescriptorSetLayout(layoutInfo);

    vk::DescriptorSetAllocateInfo setInfo;
    setInfo.setDescriptorPool(pool)
           .setSetLayouts(m_DescriptorSetLayout)
           .setDescriptorSetCount(1);

    m_DescriptorSet = device.allocateDescriptorSets(setInfo).front();
}

DescriptorSet::~DescriptorSet()
{
    const auto& device = Context::Instance().GetDevice();

    device.waitIdle();
    device.destroyDescriptorSetLayout(m_DescriptorSetLayout);
}

void DescriptorSet::UpdateUniformBuffer(Buffer* buffer, uint32_t binding) const
{
    const auto& device = Context::Instance().GetDevice();
    
    vk::DescriptorBufferInfo bufferInfo;
    bufferInfo.setBuffer(buffer->GetBuffer())
              .setOffset(0)
              .setRange(buffer->GetSize());

    vk::WriteDescriptorSet writeInfo;
    writeInfo.setDescriptorCount(1)
             .setDescriptorType(vk::DescriptorType::eUniformBuffer)
             .setBufferInfo(bufferInfo)
             .setDstBinding(binding)
             .setDstSet(m_DescriptorSet);

    device.updateDescriptorSets(writeInfo, nullptr);
}
