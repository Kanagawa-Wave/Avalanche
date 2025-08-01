#pragma once

#include <vulkan/vulkan.hpp>

#include "RenderTarget.h"
#include "Vulkan/Pipeline.h"
#include "Vulkan/RenderPass.h"

class ShadowMapRenderSystem
{
public:
    ShadowMapRenderSystem();
    ~ShadowMapRenderSystem();

private:
    std::unique_ptr<Pipeline> m_Pipeline;
    std::unique_ptr<RenderTarget> m_RenderTarget;
};
