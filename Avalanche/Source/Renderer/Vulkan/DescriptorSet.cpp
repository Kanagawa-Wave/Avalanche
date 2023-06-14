#include "DescriptorSet.h"

#include "Renderer/Context.h"

DescriptorSet::DescriptorSet(vk::DescriptorPool pool, const vk::ArrayProxy<vk::DescriptorSetLayoutBinding>& bindings)
{
    const auto& device = Context::Instance().GetDevice();

    vk::DescriptorSetLayoutCreateInfo layoutInfo;
    layoutInfo.setBindings(bindings);

    m_DescriptorSetLayout = device.createDescriptorSetLayout(layoutInfo);

    vk::DescriptorSetAllocateInfo allocateInfo;
    allocateInfo.setDescriptorPool(pool)
                .setSetLayouts(m_DescriptorSetLayout)
                .setDescriptorSetCount(1);

    m_DescriptorSet = device.allocateDescriptorSets(allocateInfo).front();
}

DescriptorSet::~DescriptorSet()
{
    const auto& device = Context::Instance().GetDevice();

    device.waitIdle();
    device.destroyDescriptorSetLayout(m_DescriptorSetLayout);
}

void DescriptorSet::UpdateUniformBuffer(const Buffer* buffer, uint32_t binding) const
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

void DescriptorSet::UpdateTexture(const Texture* texture, uint32_t binding) const
{
    const auto& device = Context::Instance().GetDevice();

    vk::DescriptorImageInfo imageInfo;
    imageInfo.setSampler(texture->GetSampler())
             .setImageView(texture->GetView())
             .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

    vk::WriteDescriptorSet writeInfo;
    writeInfo.setDescriptorCount(1)
             .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
             .setImageInfo(imageInfo)
             .setDstBinding(binding)
             .setDstSet(m_DescriptorSet);

    device.updateDescriptorSets(writeInfo, nullptr);
}
