#include "Application.h"

#include "Log/Log.h"

std::unique_ptr<Application> Application::s_Instance = nullptr;

void Application::InitInstance()
{
    s_Instance.reset(new Application());
}

void Application::Init()
{
    Log::Init();

    m_Window = std::make_unique<Window>(800, 600, "Avalanche");

    Context::Init(m_Window->GetGLFWWindow());
    
    auto& ctx = Context::Instance();
    ctx.InitSwapchain(m_Window->GetWidth(), m_Window->GetHeight());
    ctx.InitPipeline();
    ctx.InitCommandManager();
    m_Renderer = std::make_unique<Renderer>(m_Window->GetAspect());
    ctx.GetPipeline().CreateRenderPass();
    ctx.GetPipeline().CreateLayout(Renderer::PushConstantSize());
    ctx.GetSwapchain().CreateFramebuffers(m_Window->GetWidth(), m_Window->GetHeight());

    m_Triangle = std::make_unique<Mesh>("Content/bunny.obj");
    ctx.GetPipeline().CreatePipeline(m_Window->GetWidth(), m_Window->GetHeight(), m_Triangle->GetVertexBuffer().GetLayout().GetVertexInputInfo());
}

void Application::Destroy()
{
    Context::Instance().GetDevice().waitIdle();
    m_Triangle.reset();
    m_Renderer.reset();
    Context::Instance().Destroy();
}

void Application::Draw()
{
    m_Renderer->Render(m_Triangle.get());
}

void Application::Run()
{
    while (m_Window->Running())
    {
        m_Window->PollEvents();
        Draw();
    }
}
