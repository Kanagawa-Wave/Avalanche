#pragma once
#include "DescriptorSetWriter.h"

class DescriptorSet
{
public:
    DescriptorSet(vk::DescriptorSetLayout layout);
    
    void SetTexture(uint32_t binding, const Texture* texture);
    void SetUniformBuffer(uint32_t binding, const Buffer* buffer);
    void WriteAndClear();
    
    void Bind(vk::CommandBuffer commandBuffer, vk::PipelineLayout pipelineLayout, uint32_t firstSet);
    
private:
    vk::DescriptorSet m_DescriptorSet;
    vk::DescriptorSetLayout m_Layout; 
    DescriptorSetWriter m_Writer;
};
