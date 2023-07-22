#include "Renderer.h"

#include "Context.h"
#include "Core/Log.h"
#include "Window/Window.h"

#include <imgui.h>
#include <glm/glm.hpp>

#include "ImmediateContext.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_vulkan.h"
#include "Scene/Entity.h"
#include "Scene/Components/Components.h"

Renderer::Renderer(Window* window, const Camera& camera, const vk::Extent2D& viewportExtent)
    : m_Window(window), m_pCamera(&camera), m_pExtent(&viewportExtent)
{
    RenderPassCreateInfo renderPassInfo;
    renderPassInfo.setEnableDepthAttachment(false)
                  .setColorAttachmentFormat(window->GetSwapchain()->GetFormat())
                  .setLoadOp(vk::AttachmentLoadOp::eClear)
                  .setStoreOp(vk::AttachmentStoreOp::eStore)
                  .setInitialLayout(vk::ImageLayout::eUndefined)
                  .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);
    m_PresnetRenderPass = std::make_unique<RenderPass>(renderPassInfo);

    InitImGui();

    window->GetSwapchain()->CreateFramebuffers(window->GetWidth(), window->GetHeight(),
                                               m_PresnetRenderPass->GetRenderPass());

    CreateDescriptorSets();

    m_ViewportRenderTarget = std::make_unique<RenderTarget>(window->GetSwapchain()->GetFormat(),
                                                            window->GetExtent(), true);

    PipelineCreateInfo pipelineInfo;
    pipelineInfo.setVertexShader("Shaders/Unlit.vert.hlsl.spv")
                .setFragmentShader("Shaders/Unlit.frag.hlsl.spv")
                .setRenderPass(m_ViewportRenderTarget->GetRenderPass())
                .setDescriptorSetLayouts({m_GlobalSet->GetLayout(), m_TextureSet->GetLayout()})
                .setPushConstantSize(sizeof(PushConstant))
                .setVertexInputInfo(Vertex::Layout().GetVertexInputInfo());
    m_ViewportPipeline = std::make_unique<Pipeline>(pipelineInfo);

    AllocateCommandBuffer();
    CreateFence();
    CreateSemaphores();

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

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    device.destroyDescriptorPool(m_ImGuiPool);
    device.destroySemaphore(m_RenderSemaphore);
    device.destroySemaphore(m_PresentSemaphore);
    device.destroyFence(m_Fence);
}

void Renderer::SubmitScene(Scene* scene)
{
    for (const auto obj : scene->GetAllEntitiesWith<MeshComponent>())
    {
        Entity entity(obj, scene);
        m_DrawList.push_back({
            &entity.GetComponent<MeshComponent>().StaticMesh, &entity.GetComponent<TransformComponent>()
        });
    }
}

void Renderer::OnRender()
{
    m_CameraData.SetData(m_pCamera->GetProjection(), m_pCamera->GetView());
    m_CameraBuffer->SetData(&m_CameraData);
    m_TestBuffer->SetData(&m_TestData);

    const auto& device = Context::Instance().GetDevice();

    if (device.waitForFences(m_Fence, true, std::numeric_limits<std::uint64_t>::max()) != vk::Result::eSuccess)
    {
        ASSERT(0, "Failed to wait for fence")
    }
    device.resetFences(m_Fence);
    m_CommandBuffer.reset();

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
                .setY((float)m_pExtent->height)
                .setWidth((float)m_pExtent->width)
                .setHeight(-(float)m_pExtent->height)
                .setMinDepth(0.f)
                .setMaxDepth(1.f);
        scissor.setOffset({0, 0})
               .setExtent(*m_pExtent);
        m_CommandBuffer.setViewport(0, viewport);
        m_CommandBuffer.setScissor(0, scissor);
        
        for (auto [mesh, transform] : m_DrawList)
        {
            PushConstant pushConstant;
            pushConstant.model = transform->GetModelMat();
            m_CommandBuffer.pushConstants(m_ViewportPipeline->GetLayout(), vk::ShaderStageFlagBits::eVertex, 0,
                              sizeof(PushConstant),
                              &pushConstant);
            m_TextureSet->UpdateTexture(mesh->GetTexture(), 0);
            mesh->Bind(m_CommandBuffer);
            mesh->Draw(m_CommandBuffer);
        }

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
    // Context::Instance().GetGraphicsQueue().submit(submit, m_Fence);
    result = vkQueueSubmit(Context::Instance().GetGraphicsQueue(), 1, (VkSubmitInfo*)&submit, m_Fence);

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

void Renderer::ResizeViewport(vk::Extent2D extent) const
{
    m_ViewportRenderTarget->Resize(extent);
}

void* Renderer::GetViewportTextureID() const
{
    return m_ViewportRenderTarget->GetRenderTexture()->GetTextureID();
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


    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

    const float DPI_MULTIPLIER = (float)GetDpiForSystem() / 96.f;
    float fontSize = 18.0f * DPI_MULTIPLIER; // *2.0f;
    io.Fonts->AddFontFromFileTTF("Content/SF-Mono-Light.otf", fontSize);
    io.FontDefault = io.Fonts->AddFontFromFileTTF("Content/SF-Mono-Regular.otf", fontSize);

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    auto& colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4{0.1f, 0.105f, 0.11f, 1.0f};

    // Headers
    colors[ImGuiCol_Header] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
    colors[ImGuiCol_HeaderHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
    colors[ImGuiCol_HeaderActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

    // Buttons
    colors[ImGuiCol_Button] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
    colors[ImGuiCol_ButtonHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
    colors[ImGuiCol_ButtonActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

    // Frame BG
    colors[ImGuiCol_FrameBg] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
    colors[ImGuiCol_FrameBgHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
    colors[ImGuiCol_FrameBgActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
    colors[ImGuiCol_TabHovered] = ImVec4{0.38f, 0.3805f, 0.381f, 1.0f};
    colors[ImGuiCol_TabActive] = ImVec4{0.28f, 0.2805f, 0.281f, 1.0f};
    colors[ImGuiCol_TabUnfocused] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};

    // Title
    colors[ImGuiCol_TitleBg] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
    colors[ImGuiCol_TitleBgActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

    ImGui_ImplGlfw_InitForVulkan(m_Window->GetGLFWWindow(), true);

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
