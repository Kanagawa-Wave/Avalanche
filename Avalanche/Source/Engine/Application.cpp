#include "Application.h"

#include "Core/Log.h"
#include "Core/Timer.h"

std::unique_ptr<Application> Application::s_Instance = nullptr;

void Application::InitInstance()
{
    s_Instance.reset(new Application());
}

void Application::Init()
{
    Timer::Init();
    Log::Init();

    m_Window = std::make_unique<Window>(800, 600, "Avalanche");

    Context::Init(m_Window->GetGLFWWindow());
    m_Window->CreateSwapchain();
    
    m_Renderer = std::make_unique<Renderer>(m_Window.get(), true);
    m_Mesh = std::make_unique<Mesh>("Content/bunny.obj");
    m_Mesh->AddTexture("Content/bunny.png");
}

void Application::Destroy()
{
    Context::Instance().GetDevice().waitIdle();
    m_Mesh.reset();
    m_Renderer.reset();
    m_Window->Destory();
    Context::Instance().Destroy();
}

void Application::Render() const
{
    m_Renderer->Render(m_Mesh.get());
}

void Application::Run()
{
    while (m_Window->Running())
    {
        m_Window->PollEvents();
        m_Renderer->Update(Timer::Elapsed());
        Timer::Reset();
        Render();
    }
}
