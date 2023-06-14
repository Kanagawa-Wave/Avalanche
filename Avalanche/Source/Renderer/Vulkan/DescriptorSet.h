#pragma once

#include <vulkan/vulkan.hpp>

#include "Buffers.h"
#include "Renderer/Texture.h"

class DescriptorSet
{
public:
    DescriptorSet(vk::DescriptorPool pool, const vk::ArrayProxy<vk::DescriptorSetLayoutBinding>& bindings);
    ~DescriptorSet();

    void UpdateUniformBuffer(const Buffer* buffer, uint32_t binding) const;
    void UpdateTexture(const Texture* texture, uint32_t binding) const;

    vk::DescriptorSet GetDescriptorSet() const { return m_DescriptorSet; }
    vk::DescriptorSetLayout GetLayout() const { return m_DescriptorSetLayout; }

private:
    vk::DescriptorSet m_DescriptorSet;
    vk::DescriptorSetLayout m_DescriptorSetLayout;
};
