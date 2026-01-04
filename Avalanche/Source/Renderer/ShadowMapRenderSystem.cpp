#include "ShadowMapRenderSystem.h"
#include "ShadowMapRenderSystem.h"
#include "ShadowMapRenderSystem.h"

#include "Context.h"
#include "Scene/Components/Components.h"
#include "Scene/Components/Mesh.h"

ShadowMapRenderSystem::ShadowMapRenderSystem(uint32_t resolution)
    : m_Resolution(resolution)
{
    RenderTargetCreateInfo renderTargetInfo;
    renderTargetInfo.setRenderColor(false)
                    .setRenderDepth(true)
                    .setExtent({resolution, resolution})
                    .setImGuiReadable(false)
                    .setColorFormat(vk::Format::eUndefined);

    m_RenderTarget = std::make_unique<RenderTarget>(renderTargetInfo);

    PipelineCreateInfo pipelineInfo{};
    pipelineInfo.setPushConstantSize(sizeof(ShadowMapPushConstant))
    .setVertexInputInfo(ModelVertex::Layout().GetVertexInputInfo())
    .setCullMode(vk::CullModeFlagBits::eBack)
    .setRenderPass(m_RenderTarget->GetRenderPass())
    .setVertexShader("Shaders/ShadowMap.vert.hlsl.spv")
    .setFragmentShader("");

    m_Pipeline = std::make_unique<Pipeline>(pipelineInfo);
}

ShadowMapRenderSystem::~ShadowMapRenderSystem() = default;

void ShadowMapRenderSystem::Render(const Scene& scene, vk::CommandBuffer commandBuffer)
{
    m_RenderTarget->Begin(commandBuffer);
    m_Pipeline->Bind(commandBuffer);

    m_RenderTarget->End(commandBuffer);
}
