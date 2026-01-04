#pragma once

#include <deque>

#include "Buffers.h"
#include "Renderer/Texture.h"

class DescriptorSetWriter
{
public:
    DescriptorSetWriter() = default;
    void SetTexture(uint32_t binding, const Texture* texture);
    void SetUniformBuffer(uint32_t binding, const Buffer* buffer);

    void Write(vk::DescriptorSet set);
    void Clear();

private:
    std::deque<vk::DescriptorBufferInfo> m_BufferInfos;
    std::deque<vk::DescriptorImageInfo> m_ImageInfos;
    std::vector<vk::WriteDescriptorSet> m_Writes;
};
