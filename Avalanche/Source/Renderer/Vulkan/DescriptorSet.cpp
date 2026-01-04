#include "DescriptorSet.h"

#include "Renderer/Context.h"

DescriptorSet::DescriptorSet(vk::DescriptorSetLayout layout)
    : m_Layout(layout)
{
    m_DescriptorSet = Context::Instance().GetDescriptorArena()->Allocate(layout);
}

void DescriptorSet::SetTexture(uint32_t binding, const Texture* texture)
{
    m_Writer.SetTexture(binding, texture);
}

void DescriptorSet::SetUniformBuffer(uint32_t binding, const Buffer* buffer)
{
    m_Writer.SetUniformBuffer(binding, buffer);
}

void DescriptorSet::WriteAndClear()
{
    m_Writer.Write(m_DescriptorSet);
    m_Writer.Clear();
}

void DescriptorSet::Bind(vk::CommandBuffer commandBuffer, vk::PipelineLayout pipelineLayout, uint32_t firstSet)
{
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, firstSet, {m_DescriptorSet}, nullptr);
}
