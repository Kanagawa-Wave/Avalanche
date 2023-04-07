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

#include "Commands.h"


Renderer::Renderer(const Window& window)
    : m_Window(window)
{
    AllocateCommandBuffer();
    CreateFence();
    CreateSemaphores();
}

Renderer::~Renderer()
{
    const auto& device = Context::Instance().GetDevice();
    device.waitIdle();

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    device.destroyDescriptorPool(m_ImGuiData.ImGuiPool);
    device.destroySemaphore(m_RenderSemaphore);
    device.destroySemaphore(m_PresentSemaphore);
    device.destroyFence(m_Fence);
}

void Renderer::Init()
{
    InitImGui();
    InitCamera(m_Window.GetAspect());
}

void Renderer::Render(const Mesh& mesh)
{
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplVulkan_NewFrame();

    ImGui::NewFrame();

    ImGui::ShowDemoWindow();

    ImGui::Render();
    ImGui::EndFrame();

    m_Camera->OnUpdate(Timer::Elapsed());

    const glm::mat4 model = glm::scale(glm::mat4(1.f), glm::vec3(10, 10, 10));

    PushConstant pushConstant;
    pushConstant.model = model;

    m_CameraData.SetData(m_Camera->GetProjection(), m_Camera->GetView());
    m_CameraBuffer->Upload(&m_CameraData);
    m_TestBuffer->Upload(&m_TestData);

    const auto& device = Context::Instance().GetDevice();
    const auto& pipeline = Context::Instance().GetPipeline();
    const auto& swapchain = Context::Instance().GetSwapchain();

    if (device.waitForFences(m_Fence, true, std::numeric_limits<std::uint64_t>::max()) != vk::Result::eSuccess)
    {
        ASSERT(0, "Failed to wait for fence")
    }
    device.resetFences(m_Fence);

    const auto result = device.acquireNextImageKHR(swapchain.GetSwapchain(),
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
        pipeline.Bind(m_CommandBuffer);
        pipeline.BindBuffer(m_CommandBuffer, *m_CameraBuffer);
        mesh.Bind(m_CommandBuffer);
        vk::RenderPassBeginInfo renderPassBegin;
        vk::Rect2D area;
        vk::ClearValue color, depth;
        area.setOffset({0, 0})
            .setExtent(swapchain.GetExtent());
        color.setColor({0.1f, 0.1f, 0.1f, 1.0f});
        depth.setDepthStencil(1.0f);
        std::array clearValues = {color, depth};
        renderPassBegin.setRenderPass(pipeline.GetRenderPass())
                       .setRenderArea(area)
                       .setFramebuffer(swapchain.GetFramebuffer(imageIndex))
                       .setClearValues(clearValues);
        m_CommandBuffer.beginRenderPass(renderPassBegin, {});
        {
            m_CommandBuffer.pushConstants(pipeline.GetLayout(), vk::ShaderStageFlagBits::eVertex, 0, PushConstantSize(),
                                          &pushConstant);
            mesh.Draw(m_CommandBuffer);
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
           .setSwapchains(swapchain.GetSwapchain());
    if (Context::Instance().GetPresentQueue().presentKHR(present) != vk::Result::eSuccess)
    {
        ASSERT(0, "Failed to present image")
    }
}

void Renderer::OnUpdate(float deltaTime) const
{
    m_Camera->OnUpdate(deltaTime);
}

void Renderer::AllocateCommandBuffer()
{
    m_CommandBuffer = Context::Instance().GetCommandManager().AllocateCommandBuffer();
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
    auto& pipeline = Context::Instance().GetPipeline();

    m_Camera = std::make_unique<Camera>(30.0f, aspect, 0.001f, 100.0f);
    m_CameraBuffer = std::make_unique<Buffer>(vk::BufferUsageFlagBits::eUniformBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU,
                                              sizeof(CameraData));
    pipeline.SetUniformBuffer(*m_CameraBuffer, 0);

    m_TestBuffer = std::make_unique<Buffer>(vk::BufferUsageFlagBits::eUniformBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU,
                                            sizeof(TestData));
    pipeline.SetUniformBuffer(*m_TestBuffer, 0);
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
    ImGui_ImplGlfw_InitForVulkan(m_Window.GetGLFWWindow(), true);

    ImGui_ImplVulkan_InitInfo imguiInfo{};
    imguiInfo.Instance = ctx.GetInstance();
    imguiInfo.Device = ctx.GetDevice();
    imguiInfo.PhysicalDevice = ctx.GetPhysicalDevice();
    imguiInfo.Queue = ctx.GetGraphicsQueue();
    imguiInfo.DescriptorPool = m_ImGuiData.ImGuiPool;
    imguiInfo.MinImageCount = 2;
    imguiInfo.ImageCount = 2;
    imguiInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    ImGui_ImplVulkan_Init(&imguiInfo, ctx.GetPipeline().GetRenderPass());

    Commands::ImmediateSubmit([&](vk::CommandBuffer commandBuffer)
    {
        ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
    });

    ImGui_ImplVulkan_DestroyFontUploadObjects();
}
