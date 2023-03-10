#include "Engine.h"

#include "Log/Log.h"

void Engine::Init()
{
    Log::Init();

    m_Window = std::make_unique<Window>(800, 600, "Avalanche");

    Context::Init(m_Window->GetGLFWWindow());
    
    auto& ctx = Context::Instance();
    ctx.InitSwapchain(m_Window->GetWidth(), m_Window->GetHeight());
    ctx.InitPipeline();
    ctx.InitCommandManager();
    ctx.GetPipeline().CreateRenderPass();
    ctx.GetPipeline().CreateLayout();
    ctx.GetSwapchain().CreateFramebuffers(m_Window->GetWidth(), m_Window->GetHeight());
    ctx.GetPipeline().CreatePipeline(m_Window->GetWidth(), m_Window->GetHeight());

    m_Renderer = std::make_unique<Renderer>();
}

void Engine::Destroy()
{
    m_Renderer.reset();
    Context::Instance().Destroy();
}

void Engine::Draw()
{
    m_Renderer->Render();
}

void Engine::Run()
{
    while (m_Window->Running())
    {
        m_Window->PollEvents();
        Draw();
    }
}
