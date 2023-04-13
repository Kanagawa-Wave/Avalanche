#pragma once

#include <vulkan/vulkan.hpp>

class RenderPass
{
public:
    RenderPass(vk::Format colorFormat);
    ~RenderPass();

    const vk::RenderPass& GetRenderPass() const { return m_RenderPass; }

private:
    vk::RenderPass m_RenderPass;
};
