#include "ShadowMapRenderSystem.h"
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
    
    m_UniformBuffer = std::make_unique<Buffer>(
        vk::BufferUsageFlagBits::eUniformBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU, sizeof(ShadowMapUniformBuffer));
    m_ShadowMapGlobalSet = std::make_unique<DescriptorSet>(
        m_Pipeline->GetShader()->GetShaderResourceLayout()->GetLayout(Context::GLOBAL_SET));
}

ShadowMapRenderSystem::~ShadowMapRenderSystem() = default;

void ShadowMapRenderSystem::Render(const Scene& scene, vk::CommandBuffer commandBuffer)
{
    auto lightView = scene.m_Registry.view<TransformComponent, DirectionalLightComponent>();
    if (lightView.begin() != lightView.end())
    {
        auto [lightTransform, light] = lightView.get<TransformComponent, DirectionalLightComponent>(lightView.front());
        m_UniformBufferData.ViewProjection = light.CalcLightSpaceMatrix(lightTransform.Translation);
        m_UniformBuffer->SetData(&m_UniformBufferData);
        m_ShadowMapGlobalSet->SetUniformBuffer(0, m_UniformBuffer.get());
        m_ShadowMapGlobalSet->WriteAndClear();
    
        m_RenderTarget->Begin(commandBuffer);
        m_Pipeline->Bind(commandBuffer);
        m_ShadowMapGlobalSet->Bind(commandBuffer, m_Pipeline->GetLayout(), 0);
        
        vk::Viewport viewport;
        vk::Rect2D scissor;
        viewport.setX(0.f)
            .setY((float)m_Resolution)
            .setWidth((float)m_Resolution)
            .setHeight(-(float)m_Resolution)
            .setMinDepth(0.f)
            .setMaxDepth(1.f);
        scissor.setOffset({ 0, 0 })
            .setExtent({m_Resolution, m_Resolution});
        commandBuffer.setViewport(0, viewport);
        commandBuffer.setScissor(0, scissor);
    
        auto meshView = scene.m_Registry.view<TransformComponent, StaticMeshComponent>();
        for (auto entity : meshView)
        {
            auto [transform, mesh] = meshView.get<TransformComponent, StaticMeshComponent>(entity);
            if (mesh.Visible)
            {
                m_PushConstant.Model = transform.GetModelMat();
                commandBuffer.pushConstants(m_Pipeline->GetLayout(), vk::ShaderStageFlagBits::eVertex, 0,
                    sizeof(ShadowMapPushConstant),
                    &m_PushConstant);
	
                mesh.StaticMesh->Bind(commandBuffer);
                mesh.StaticMesh->Draw(commandBuffer);
            }
        }

        m_RenderTarget->End(commandBuffer);
    }
}
