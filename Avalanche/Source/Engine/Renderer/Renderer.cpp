#include "Renderer.h"

#include "Context.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Timer.h"
#include "Engine/Window/Window.h"

#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "ImmediateContext.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_vulkan.h"

Renderer::Renderer(Window* window, bool enableImGui)
    : m_Window(window), m_EnableImGui(enableImGui)
{
    RenderPassCreateInfo renderPassInfo;
    renderPassInfo.setEnableDepthAttachment(false)
                  .setColorAttachmentFormat(window->GetSwapchain()->GetFormat())
                  .setLoadOp(vk::AttachmentLoadOp::eClear)
                  .setStoreOp(vk::AttachmentStoreOp::eStore)
                  .setInitialLayout(vk::ImageLayout::eUndefined)
                  .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);
    m_PresnetRenderPass = std::make_unique<RenderPass>(renderPassInfo);
    
    if (m_EnableImGui)
    {
        InitImGui();
    }

    window->GetSwapchain()->CreateFramebuffers(window->GetWidth(), window->GetHeight(),
                                               m_PresnetRenderPass->GetRenderPass());

    CreateDescriptorSets();

    m_ViewportRenderTarget = std::make_unique<RenderTarget>(window->GetSwapchain()->GetFormat(),
                                                            window->GetExtent(), true);
    
    PipelineCreateInfo pipelineInfo;
    pipelineInfo.setVertexShader("Shaders/Triangle.vert.spv")
                .setFragmentShader("Shaders/Triangle.frag.spv")
                .setRenderPass(m_ViewportRenderTarget->GetRenderPass())
                .setDescriptorSetLayouts({m_GlobalSet->GetLayout(), m_TextureSet->GetLayout()})
                .setPushConstantSize(sizeof(PushConstant))
                .setVertexInputInfo(Vertex::Layout().GetVertexInputInfo());
    m_ViewportPipeline = std::make_unique<Pipeline>(pipelineInfo);

    m_ViewportExtent = m_Window->GetExtent();

    AllocateCommandBuffer();
    CreateFence();
    CreateSemaphores();

    m_Camera = std::make_unique<Camera>(m_ViewportExtent, 30.0f, 0.001f, 100.0f);
    m_CameraBuffer = std::make_unique<Buffer>(vk::BufferUsageFlagBits::eUniformBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU,
                                              sizeof(CameraData));
    m_GlobalSet->UpdateUniformBuffer(m_CameraBuffer.get(), 0);

    m_TestBuffer = std::make_unique<Buffer>(vk::BufferUsageFlagBits::eUniformBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU,
                                            sizeof(TestData));
    m_GlobalSet->UpdateUniformBuffer(m_TestBuffer.get(), 1);
}

Renderer::~Renderer()
{
    const auto& device = Context::Instance().GetDevice();
    device.waitIdle();

    if (m_EnableImGui)
    {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
    }

    device.destroyDescriptorPool(m_ImGuiPool);
    device.destroySemaphore(m_RenderSemaphore);
    device.destroySemaphore(m_PresentSemaphore);
    device.destroyFence(m_Fence);
}

void Renderer::Begin()
{
}

void Renderer::Render(const Mesh* mesh)
{
    const glm::mat4 model = glm::scale(glm::mat4(1.f), glm::vec3(10, 10, 10));

    PushConstant pushConstant;
    pushConstant.model = model;

    m_CameraData.SetData(m_Camera->GetProjection(), m_Camera->GetView());
    m_CameraBuffer->SetData(&m_CameraData);
    m_TestBuffer->SetData(&m_TestData);

    const auto& device = Context::Instance().GetDevice();

    if (device.waitForFences(m_Fence, true, std::numeric_limits<std::uint64_t>::max()) != vk::Result::eSuccess)
    {
        ASSERT(0, "Failed to wait for fence")
    }
    device.resetFences(m_Fence);
    m_CommandBuffer.reset();

    m_TextureSet->UpdateTexture(mesh->GetTexture(), 0);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device, m_Window->GetSwapchain()->GetSwapchain(),
                                            std::numeric_limits<uint64_t>::max(),
                                            m_PresentSemaphore, VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        m_Window->RecreateSwapchain(m_PresnetRenderPass->GetRenderPass());
        return;
    }
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        ASSERT(0, "Failed to acquire next swapchain image")
    }

    vk::CommandBufferBeginInfo commandBufferBegin;
    commandBufferBegin.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    m_CommandBuffer.begin(commandBufferBegin);
    {
        // Pass #1: Render to viewport
        m_ViewportRenderTarget->Begin(m_CommandBuffer);
        m_ViewportPipeline->Bind(m_CommandBuffer);
        m_ViewportPipeline->BindDescriptorSets(m_CommandBuffer, {
                                                   m_GlobalSet->GetDescriptorSet(), m_TextureSet->GetDescriptorSet()
                                               });

        vk::Viewport viewport;
        vk::Rect2D scissor;
        viewport.setX(0.f)
                .setY((float)m_ViewportExtent.height)
                .setWidth((float)m_ViewportExtent.width)
                .setHeight(-(float)m_ViewportExtent.height)
                .setMinDepth(0.f)
                .setMaxDepth(1.f);
        scissor.setOffset({0, 0})
               .setExtent(m_ViewportExtent);
        m_CommandBuffer.setViewport(0, viewport);
        m_CommandBuffer.setScissor(0, scissor);

        m_CommandBuffer.pushConstants(m_ViewportPipeline->GetLayout(), vk::ShaderStageFlagBits::eVertex, 0,
                                      sizeof(PushConstant),
                                      &pushConstant);
        mesh->Bind(m_CommandBuffer);
        mesh->Draw(m_CommandBuffer);
        m_ViewportRenderTarget->End(m_CommandBuffer);

        // Pass #2: Render ImGui
        vk::RenderPassBeginInfo renderPassBegin;
        vk::Rect2D area;
        vk::ClearValue color, depth;
        area.setOffset({0, 0})
            .setExtent(m_Window->GetSwapchain()->GetExtent());
        color.setColor({0.1f, 0.1f, 0.1f, 1.0f});
        depth.setDepthStencil(1.0f);
        std::array clearValues = {color, depth};
        renderPassBegin.setRenderPass(m_PresnetRenderPass->GetRenderPass())
                       .setRenderArea(area)
                       .setFramebuffer(m_Window->GetSwapchain()->GetFramebuffer(imageIndex))
                       .setClearValues(clearValues);

        m_CommandBuffer.beginRenderPass(renderPassBegin, {});
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_CommandBuffer);
        m_CommandBuffer.endRenderPass();
    }
    m_CommandBuffer.end();

    vk::SubmitInfo submit;
    vk::PipelineStageFlags flags = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    submit.setCommandBuffers(m_CommandBuffer)
          .setWaitSemaphores(m_PresentSemaphore)
          .setWaitDstStageMask(flags)
          .setSignalSemaphores(m_RenderSemaphore);
    Context::Instance().GetGraphicsQueue().submit(submit, m_Fence);

    vk::PresentInfoKHR present;
    present.setWaitSemaphores(m_RenderSemaphore)
           .setImageIndices(imageIndex)
           .setSwapchains(m_Window->GetSwapchain()->GetSwapchain());

    result = vkQueuePresentKHR(Context::Instance().GetPresentQueue(), (VkPresentInfoKHR*)&present);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Window->SwapchainOutdated())
    {
        m_Window->RecreateSwapchain(m_PresnetRenderPass->GetRenderPass());
    }
    else if (result != VK_SUCCESS)
    {
        ASSERT(0, "Failed to present image")
    }
}

void Renderer::OnUpdate(float deltaTime)
{
    m_Camera->OnUpdate(deltaTime);

    if (m_EnableImGui)
        OnImGuiUpdate();
}

void Renderer::AllocateCommandBuffer()
{
    m_CommandBuffer = Context::Instance().GetCommandManager()->AllocateCommandBuffer();
}

void Renderer::CreateSemaphores()
{
    const auto& device = Context::Instance().GetDevice();
    const vk::SemaphoreCreateInfo semaphoreInfo;
    m_RenderSemaphore = device.createSemaphore(semaphoreInfo);
    m_PresentSemaphore = device.createSemaphore(semaphoreInfo);
}

void Renderer::CreateFence()
{
    vk::FenceCreateInfo fenceInfo;
    fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
    m_Fence = Context::Instance().GetDevice().createFence(fenceInfo);
}

void Renderer::CreateDescriptorSets()
{
    const auto& pool = Context::Instance().GetDescriptorPool();

    vk::DescriptorSetLayoutBinding globalSetBindings[] = {
        {0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex},
        {1, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eFragment}
    };
    m_GlobalSet = std::make_unique<DescriptorSet>(pool, globalSetBindings);

    vk::DescriptorSetLayoutBinding textureSetBindings[] = {
        {0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment}
    };
    m_TextureSet = std::make_unique<DescriptorSet>(pool, textureSetBindings);
}

void Renderer::InitImGui()
{
    const auto& ctx = Context::Instance();

    std::vector<vk::DescriptorPoolSize> poolSizes =
    {
        {vk::DescriptorType::eSampler, 1000},
        {vk::DescriptorType::eCombinedImageSampler, 1000},
        {vk::DescriptorType::eSampledImage, 1000},
        {vk::DescriptorType::eStorageImage, 1000},
        {vk::DescriptorType::eUniformTexelBuffer, 1000},
        {vk::DescriptorType::eStorageTexelBuffer, 1000},
        {vk::DescriptorType::eUniformBuffer, 1000},
        {vk::DescriptorType::eStorageBuffer, 1000},
        {vk::DescriptorType::eUniformBufferDynamic, 1000},
        {vk::DescriptorType::eStorageBufferDynamic, 1000},
        {vk::DescriptorType::eInputAttachment, 1000}
    };

    vk::DescriptorPoolCreateInfo poolInfo;
    poolInfo.setMaxSets(1000)
            .setPoolSizes(poolSizes);

    m_ImGuiPool = ctx.GetDevice().createDescriptorPool(poolInfo);

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(m_Window->GetGLFWWindow(), true);

    // Enable docking support
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui_ImplVulkan_InitInfo imguiInfo{};
    imguiInfo.Instance = ctx.GetInstance();
    imguiInfo.Device = ctx.GetDevice();
    imguiInfo.PhysicalDevice = ctx.GetPhysicalDevice();
    imguiInfo.Queue = ctx.GetGraphicsQueue();
    imguiInfo.DescriptorPool = m_ImGuiPool;
    imguiInfo.MinImageCount = m_Window->GetSwapchain()->GetImageCount();
    imguiInfo.ImageCount = m_Window->GetSwapchain()->GetImageCount();
    imguiInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    ImGui_ImplVulkan_Init(&imguiInfo, m_PresnetRenderPass->GetRenderPass());

    ImmediateContext::Submit([&](vk::CommandBuffer commandBuffer)
    {
        ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
    });

    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void Renderer::OnImGuiUpdate()
{
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplVulkan_NewFrame();

    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::Begin("Config");
    ImGui::End();
    
    ImGui::Begin("Details");
    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Viewport");
    const ImVec2 viewportExtent = ImGui::GetContentRegionAvail();
    m_ViewportExtent.setWidth((uint32_t)viewportExtent.x)
                    .setHeight((uint32_t)viewportExtent.y);
    m_Camera->Resize(m_ViewportExtent);
    m_ViewportRenderTarget->Resize(m_ViewportExtent);
    ImGui::Image(m_ViewportRenderTarget->GetRenderTexture()->GetTextureID(), {
                     (float)m_ViewportRenderTarget->GetExtent().width,
                     (float)m_ViewportRenderTarget->GetExtent().height
                 });
    ImGui::End();
    ImGui::PopStyleVar();
    
    ImGui::Render();
    ImGui::EndFrame();
}
