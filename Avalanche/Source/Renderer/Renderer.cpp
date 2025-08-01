#include "Renderer.h"
#include "Renderer.h"
#include "Renderer.h"
#include "Renderer.h"
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
#include "Scene/Components/Billboard.h"
#include "Scene/Components/Components.h"

Renderer::Renderer(Window* window, const vk::Extent2D& viewportExtent)
	: m_Window(window), m_pExtent(&viewportExtent)
{
	CreateLayout();

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

	RenderTargetCreateInfo renderTargetInfo;
	renderTargetInfo.setColorFormat(window->GetSwapchain()->GetFormat())
					.setExtent(window->GetExtent())
					.setRenderColor(true)
					.setRenderDepth(true)
					.setImGuiReadable(true);
	m_ViewportRenderTarget = std::make_unique<RenderTarget>(renderTargetInfo);

	const ShaderDataLayout mainGlobalSetLayout[] = {
		{0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex, sizeof(CameraDataVert)},
		{1, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eFragment, sizeof(PointLightData)},
		{2, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eFragment, sizeof(CameraDataFrag)}
	};
	PipelineCreateInfo pipelineInfo;
	pipelineInfo.setVertexShader("Shaders/Phong.vert.hlsl.spv")
				.setFragmentShader("Shaders/Phong.frag.hlsl.spv")
				.setGlobalSetLayout(mainGlobalSetLayout, EDescriptorSetLayoutType::MainGlobalSet)
				.setRenderPass(m_ViewportRenderTarget->GetRenderPass())
				.setCullMode(vk::CullModeFlagBits::eBack)
				.setPushConstantSize(sizeof(MainPushConstant))
				.setVertexInputInfo(ModelVertex::Layout().GetVertexInputInfo());
	m_MainPipeline = std::make_unique<Pipeline>(pipelineInfo);

	const ShaderDataLayout billboardGlobalSetLayout[] = {
		{0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex, sizeof(CameraDataVert)},
	};
	pipelineInfo.setVertexShader("Shaders/Billboard.vert.hlsl.spv")
			.setFragmentShader("Shaders/Billboard.frag.hlsl.spv")
			.setGlobalSetLayout(billboardGlobalSetLayout, EDescriptorSetLayoutType::BillboardGlobalSet)
			.setRenderPass(m_ViewportRenderTarget->GetRenderPass())
			.setCullMode(vk::CullModeFlagBits::eNone)
			.setPushConstantSize(sizeof(BillboardPushConstant))
			.setVertexInputInfo(Billboard::Layout().GetVertexInputInfo());
	m_BillboardPipeline = std::make_unique<Pipeline>(pipelineInfo);

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

	LOG_T("Destroying VkDescriptorPool {}", fmt::ptr((VkDescriptorPool)m_ImGuiPool))
	device.destroyDescriptorPool(m_ImGuiPool);

	LOG_T("Destroying VkSemaphore {}", fmt::ptr((VkSemaphore)m_RenderSemaphore))
	device.destroySemaphore(m_RenderSemaphore);

	LOG_T("Destroying VkSemaphore {}", fmt::ptr((VkSemaphore)m_PresentSemaphore))
	device.destroySemaphore(m_PresentSemaphore);

	LOG_T("Destroying VkFence {}", fmt::ptr((VkFence)m_Fence))
	device.destroyFence(m_Fence);
}

void Renderer::Begin(const Camera& camera, const Scene& scene)
{
	//m_CameraBufferVert->SetData(&m_CameraDataVert);

	m_CameraDataVert.SetData(camera.GetProjection(), camera.GetView());
	m_MainPipeline->SetShaderBufferData(0, &m_CameraDataVert);
	m_BillboardPipeline->SetShaderBufferData(0, &m_CameraDataVert);

	const auto view = scene.GetAllEntitiesWith<PointLightComponent, TransformComponent>();
	m_PointLightData.m_PointLightCount = 0;
	for (const auto entity : view)
	{
		auto [pointLight, transform] = view.get<PointLightComponent, TransformComponent>(entity);
		m_PointLightData.SetData(transform.Translation, pointLight.Color);
	}
	m_MainPipeline->SetShaderBufferData(1, &m_PointLightData);

	m_CameraDataFrag.SetData(camera.GetPosition());
	m_MainPipeline->SetShaderBufferData(2, &m_CameraDataFrag);

	const auto& device = Context::Instance().GetDevice();

	if (device.waitForFences(m_Fence, true, std::numeric_limits<std::uint64_t>::max()) != vk::Result::eSuccess)
	{
		ASSERT(0, "Failed to wait for fence")
	}
	device.resetFences(m_Fence);
	m_CommandBuffer.reset();

	VkResult result = vkAcquireNextImageKHR(device, m_Window->GetSwapchain()->GetSwapchain(),
		std::numeric_limits<uint64_t>::max(),
		m_PresentSemaphore, VK_NULL_HANDLE, &m_ImageIndex);

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

	// Pass #1: Render to viewport
	m_ViewportRenderTarget->Begin(m_CommandBuffer);
	m_MainPipeline->Bind(m_CommandBuffer);

	vk::Viewport viewport;
	vk::Rect2D scissor;
	viewport.setX(0.f)
		.setY((float)m_pExtent->height)
		.setWidth((float)m_pExtent->width)
		.setHeight(-(float)m_pExtent->height)
		.setMinDepth(0.f)
		.setMaxDepth(1.f);
	scissor.setOffset({ 0, 0 })
		.setExtent(*m_pExtent);
	m_CommandBuffer.setViewport(0, viewport);
	m_CommandBuffer.setScissor(0, scissor);
}

void Renderer::DrawModel(const TransformComponent& transform, const StaticMeshComponent& mesh) const
{
	MainPushConstant pushConstant;

	pushConstant.model = transform.GetModelMat();
	pushConstant.normalMat = glm::transpose(glm::inverse(pushConstant.model));
	m_CommandBuffer.pushConstants(m_MainPipeline->GetLayout(), vk::ShaderStageFlagBits::eVertex, 0,
		sizeof(MainPushConstant),
		&pushConstant);
	mesh.StaticMesh->Bind(m_CommandBuffer, m_MainPipeline->GetLayout());
	mesh.StaticMesh->Draw(m_CommandBuffer);
}

void Renderer::DrawBillboard(const TransformComponent& transform, const BillboardComponent& billboard) const
{
	BillboardPushConstant pushConstant;

	pushConstant.position = transform.Translation;
	m_CommandBuffer.pushConstants(m_BillboardPipeline->GetLayout(), vk::ShaderStageFlagBits::eVertex, 0,
		sizeof(BillboardPushConstant),
		&pushConstant);
	billboard.BillboardObject->Bind(m_CommandBuffer, m_BillboardPipeline->GetLayout());
	billboard.BillboardObject->Draw(m_CommandBuffer);
}

void Renderer::End()
{
	m_ViewportRenderTarget->End(m_CommandBuffer);

	// Pass #2: Render ImGui
	vk::RenderPassBeginInfo renderPassBegin;
	vk::Rect2D area;
	vk::ClearValue color, depth;
	area.setOffset({ 0, 0 })
		.setExtent(m_Window->GetSwapchain()->GetExtent());
	color.setColor({ 0.1f, 0.1f, 0.1f, 1.0f });
	depth.setDepthStencil(1.0f);
	std::array clearValues = { color, depth };
	renderPassBegin.setRenderPass(m_PresnetRenderPass->GetRenderPass())
		.setRenderArea(area)
		.setFramebuffer(m_Window->GetSwapchain()->GetFramebuffer(m_ImageIndex))
		.setClearValues(clearValues);

	m_CommandBuffer.beginRenderPass(renderPassBegin, {});
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_CommandBuffer);
	m_CommandBuffer.endRenderPass();

	m_CommandBuffer.end();

	vk::SubmitInfo submit;
	vk::PipelineStageFlags flags = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	submit.setCommandBuffers(m_CommandBuffer)
		.setWaitSemaphores(m_PresentSemaphore)
		.setWaitDstStageMask(flags)
		.setSignalSemaphores(m_RenderSemaphore);

	VkResult result = vkQueueSubmit(Context::Instance().GetGraphicsQueue(), 1, (VkSubmitInfo*)&submit, m_Fence);

	vk::PresentInfoKHR present;
	present.setWaitSemaphores(m_RenderSemaphore)
		.setImageIndices(m_ImageIndex)
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

void Renderer::Render(const Camera& camera, const Scene& scene)
{
	Begin(camera, scene);
	auto meshView = scene.m_Registry.view<TransformComponent, StaticMeshComponent>();
	for (auto entity : meshView)
	{
		auto [transform, model] = meshView.get<TransformComponent, StaticMeshComponent>(entity);
		if (model.Visible)
		{
			DrawModel(transform, model);
		}
	}

	m_BillboardPipeline->Bind(m_CommandBuffer);
	auto billboardView = scene.m_Registry.view<TransformComponent, BillboardComponent>();
	for (auto entity : billboardView)
	{
		auto [transform, billboard] = billboardView.get<TransformComponent, BillboardComponent>(entity);
		DrawBillboard(transform, billboard);
	}
	End();
}

void Renderer::OnResize(vk::Extent2D extent) const
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
	LOG_T("VkSemaphore {0} created successfully", fmt::ptr((VkSemaphore)m_RenderSemaphore))
	m_PresentSemaphore = device.createSemaphore(semaphoreInfo);
	LOG_T("VkSemaphore {0} created successfully", fmt::ptr((VkSemaphore)m_PresentSemaphore))
}

void Renderer::CreateFence()
{
	vk::FenceCreateInfo fenceInfo;
	fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
	m_Fence = Context::Instance().GetDevice().createFence(fenceInfo);
	LOG_T("VkFence {0} created successfully", fmt::ptr((VkFence)m_Fence))
}

void Renderer::CreateLayout()
{
	Context& instance = Context::Instance();
	
	constexpr vk::DescriptorSetLayoutBinding mainGlobalBinding[] = {
		{0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex},
		{1, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eFragment},
		{2, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eFragment}
	};

	constexpr vk::DescriptorSetLayoutBinding billboardGlobalBinding[] = {
		{0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex},
	};

	constexpr vk::DescriptorSetLayoutBinding perModelBinding[] = {
		{0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment}
	};
	
	instance.InitDescriptorSetBuilder(3);
	instance.GetDescriptorSetBuilder()->SetLayout(EDescriptorSetLayoutType::MainGlobalSet, mainGlobalBinding);
	instance.GetDescriptorSetBuilder()->SetLayout(EDescriptorSetLayoutType::BillboardGlobalSet, billboardGlobalBinding);
	instance.GetDescriptorSetBuilder()->SetLayout(EDescriptorSetLayoutType::PerModelSet, perModelBinding);
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
	LOG_T("VkDescriptorPool {} created successfully", fmt::ptr((VkDescriptorPool)m_ImGuiPool))


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
	colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

	// Headers
	colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
	colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
	colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	// Buttons
	colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
	colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
	colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	// Frame BG
	colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
	colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
	colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	// Tabs
	colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
	colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
	colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

	// Title
	colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

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

	ImmediateContext::Instance().Submit([&](vk::CommandBuffer commandBuffer)
		{
			ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
		});

	ImGui_ImplVulkan_DestroyFontUploadObjects();
}
