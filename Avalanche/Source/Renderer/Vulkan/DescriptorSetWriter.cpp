#include "DescriptorSetWriter.h"

#include <entt/entt.hpp>

#include "Renderer/Context.h"

void DescriptorSetWriter::SetTexture(uint32_t binding, const Texture* texture)
{
    vk::DescriptorImageInfo imageInfo;
    imageInfo.setSampler(texture->GetSampler())
        .setImageView(texture->GetView())
        .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
    m_ImageInfos.push_back(imageInfo);
    vk::WriteDescriptorSet write;
    write.setDescriptorCount(1)
        .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
        .setDstSet(VK_NULL_HANDLE)
        .setDstBinding(binding)
        .setImageInfo(m_ImageInfos.back());
    m_Writes.push_back(write);
}

void DescriptorSetWriter::SetUniformBuffer(uint32_t binding, const Buffer* buffer)
{
    vk::DescriptorBufferInfo bufferInfo;
    bufferInfo.setBuffer(buffer->GetBuffer())
        .setOffset(0)
        .setRange(buffer->GetSize());
    m_BufferInfos.push_back(bufferInfo);
    vk::WriteDescriptorSet write;
    write.setDescriptorCount(1)
        .setDescriptorType(vk::DescriptorType::eUniformBuffer)
        .setDstSet(VK_NULL_HANDLE)
        .setDstBinding(binding)
        .setBufferInfo(m_BufferInfos.back());
    m_Writes.push_back(write);
}

void DescriptorSetWriter::Write(vk::DescriptorSet set)
{
    auto device = Context::Instance().GetDevice();
    
    for (vk::WriteDescriptorSet& write : m_Writes) {
        write.dstSet = set;
    }
    
    device.updateDescriptorSets(m_Writes, {});
}

void DescriptorSetWriter::Clear()
{
    m_ImageInfos.clear();
    m_BufferInfos.clear();
    m_Writes.clear();
}
