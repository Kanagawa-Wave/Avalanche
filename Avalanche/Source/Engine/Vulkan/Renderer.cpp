#include "Renderer.h"

#include "Context.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Timer.h"
#include "Engine/Window/Window.h"

#include <imgui.h>
#include <Engine/Vulkan/ImGui/imgui_impl_glfw.h>
#include <Engine/Vulkan/ImGui/imgui_impl_vulkan.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "ImmediateContext.h"

Renderer::Renderer(Window* window, bool enableImGui)
    : m_Window(window), m_EnableImGui(enableImGui)
{
    m_RenderPass = std::make_unique<RenderPass>(window->GetSwapchain()->GetFormat());
    window->GetSwapchain()->CreateFramebuffers(window->GetWidth(), window->GetHeight(), m_RenderPass->GetRenderPass());
    m_Pipeline = std::make_unique<Pipeline>("Shaders/Triangle.vert.spv",
                                            "Shaders/Triangle.frag.spv",
                                            window->GetExtent(),
                                            PushConstantSize(),
                                            Vertex::Layout().GetVertexInputInfo(),
                                            m_RenderPass->GetRenderPass());
    AllocateCommandBuffer();
    CreateFence();
    CreateSemaphores();

    if (m_EnableImGui)
    {
        InitImGui();
        InitImGUIObjects();
    }

    InitCamera(m_Window->GetAspect());
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

    device.destroyDescriptorPool(m_ImGuiData.ImGuiPool);
    device.destroySemaphore(m_RenderSemaphore);
    device.destroySemaphore(m_PresentSemaphore);
    device.destroyFence(m_Fence);

    m_RenderPass.reset();
    m_Pipeline.reset();
}

void Renderer::Render(const Mesh* mesh)
{
    const glm::mat4 model = glm::scale(glm::mat4(1.f), glm::vec3(10, 10, 10));

    PushConstant pushConstant;
    pushConstant.model = model;

    m_CameraData.SetData(m_Camera->GetProjection(), m_Camera->GetView());
    m_CameraBuffer->Upload(&m_CameraData);
    m_TestBuffer->Upload(&m_TestData);

    const auto& device = Context::Instance().GetDevice();

    if (device.waitForFences(m_Fence, true, std::numeric_limits<std::uint64_t>::max()) != vk::Result::eSuccess)
    {
        ASSERT(0, "Failed to wait for fence")
    }
    device.resetFences(m_Fence);

    const auto result = device.acquireNextImageKHR(m_Window->GetSwapchain()->GetSwapchain(),
                                                   std::numeric_limits<uint64_t>::max(), m_PresentSemaphore);
    if (result.result != vk::Result::eSuccess)
    {
        ASSERT(0, "Failed to acquire next swapchain image")
    }

    uint32_t imageIndex = result.value;

    m_CommandBuffer.reset();

    vk::CommandBufferBeginInfo commandBufferBegin;
    commandBufferBegin.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    m_CommandBuffer.begin(commandBufferBegin);
    {
        m_Pipeline->Bind(m_CommandBuffer);
        m_Pipeline->BindBuffer(m_CommandBuffer, m_CameraBuffer.get());
        mesh->Bind(m_CommandBuffer);
        vk::RenderPassBeginInfo renderPassBegin;
        vk::Rect2D area;
        vk::ClearValue color, depth;
        area.setOffset({0, 0})
            .setExtent(m_Window->GetSwapchain()->GetExtent());
        color.setColor({0.1f, 0.1f, 0.1f, 1.0f});
        depth.setDepthStencil(1.0f);
        std::array clearValues = {color, depth};
        renderPassBegin.setRenderPass(m_RenderPass->GetRenderPass())
                       .setRenderArea(area)
                       .setFramebuffer(m_Window->GetSwapchain()->GetFramebuffer(imageIndex))
                       .setClearValues(clearValues);
        m_CommandBuffer.beginRenderPass(renderPassBegin, {});
        {
            m_CommandBuffer.pushConstants(m_Pipeline->GetLayout(), vk::ShaderStageFlagBits::eVertex, 0,
                                          PushConstantSize(),
                                          &pushConstant);
            mesh->Draw(m_CommandBuffer);

            if (m_EnableImGui)
                ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_CommandBuffer);
        }
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
    if (Context::Instance().GetPresentQueue().presentKHR(present) != vk::Result::eSuccess)
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

void Renderer::InitCamera(float aspect)
{
    m_Camera = std::make_unique<Camera>(30.0f, aspect, 0.001f, 100.0f);
    m_CameraBuffer = std::make_unique<Buffer>(vk::BufferUsageFlagBits::eUniformBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU,
                                              sizeof(CameraData));
    m_Pipeline->SetUniformBuffer(m_CameraBuffer.get(), 0);

    m_TestBuffer = std::make_unique<Buffer>(vk::BufferUsageFlagBits::eUniformBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU,
                                            sizeof(TestData));
    m_Pipeline->SetUniformBuffer(m_TestBuffer.get(), 0);
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

    m_ImGuiData.ImGuiPool = ctx.GetDevice().createDescriptorPool(poolInfo);

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(m_Window->GetGLFWWindow(), true);

    ImGui_ImplVulkan_InitInfo imguiInfo{};
    imguiInfo.Instance = ctx.GetInstance();
    imguiInfo.Device = ctx.GetDevice();
    imguiInfo.PhysicalDevice = ctx.GetPhysicalDevice();
    imguiInfo.Queue = ctx.GetGraphicsQueue();
    imguiInfo.DescriptorPool = m_ImGuiData.ImGuiPool;
    imguiInfo.MinImageCount = 2;
    imguiInfo.ImageCount = 2;
    imguiInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    ImGui_ImplVulkan_Init(&imguiInfo, m_RenderPass->GetRenderPass());

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

    ImGui::ShowDemoWindow();

    ImGui::Render();
    ImGui::EndFrame();
}

void Renderer::InitImGUIObjects()
{
    const auto& device = Context::Instance().GetDevice();
    const auto& allocator = Context::Instance().GetAllocator();

    const uint32_t imageCount = m_Window->GetSwapchain()->GetImageCount();
    m_ImGuiData.m_ViewportImages.resize(imageCount);
    m_ImGuiData.m_Allocations.resize(imageCount);
    m_ImGuiData.m_ViewportImageViews.resize(imageCount);

    for (uint32_t i = 0; i < imageCount; i++)
    {
        vk::ImageCreateInfo imageInfo;
        imageInfo.setImageType(vk::ImageType::e2D)
                 .setFormat(m_Window->GetSwapchain()->GetFormat())
                 .setExtent(vk::Extent3D(m_Window->GetSwapchain()->GetExtent(), 1.0f))
                 .setArrayLayers(1)
                 .setMipLevels(1)
                 .setSamples(vk::SampleCountFlagBits::e1)
                 .setInitialLayout(vk::ImageLayout::eUndefined)
                 .setTiling(vk::ImageTiling::eOptimal)
                 .setUsage(vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled);

        VmaAllocationCreateInfo allocationInfo{};
        allocationInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        allocationInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        vmaCreateImage(allocator, (VkImageCreateInfo*)&imageInfo, &allocationInfo,
                       (VkImage*)&m_ImGuiData.m_ViewportImages[i], &m_ImGuiData.m_Allocations[i], nullptr);

        vk::ImageMemoryBarrier barrier;
        barrier.setImage(m_ImGuiData.m_ViewportImages[i])
               .setSrcAccessMask(vk::AccessFlagBits::eTransferRead)
               .setDstAccessMask(vk::AccessFlagBits::eMemoryRead)
               .setOldLayout(vk::ImageLayout::eUndefined)
               .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
               .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});

        vk::CommandBuffer commandBuffer = ImmediateContext::GetCommandBuffer();
        ImmediateContext::Begin();
        commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer,
                                      vk::DependencyFlagBits::eByRegion, nullptr, nullptr, barrier);
        ImmediateContext::End();
    }

    for (uint32_t i = 0; i < imageCount; i++)
    {
        vk::ImageViewCreateInfo viewInfo;
        viewInfo.setImage(m_ImGuiData.m_ViewportImages[i])
                .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1})
                .setFormat(m_Window->GetSwapchain()->GetFormat())
                .setViewType(vk::ImageViewType::e2D);

        m_ImGuiData.m_ViewportImageViews[i] = device.createImageView(viewInfo);
    }
}
