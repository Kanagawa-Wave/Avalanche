#include "Application.h"

#include "Core/Input.h"
#include "Core/Log.h"
#include "Core/Timer.h"
#include "Renderer/Context.h"

Application* Application::s_Application = CreateApplication();

Application::Application()
{
    Timer::Init();
    Log::Init();

    m_Window = std::make_unique<Window>(1920, 1080, "Avalanche");

    Context::Init(m_Window->GetGLFWWindow());
    m_Window->CreateSwapchain();
}

Application::~Application()
{
    const auto& device = Context::Instance().GetDevice();
    device.waitIdle();
    m_Renderer.reset();
    m_Window->Destory();
    Context::Instance().Destroy();
}

void Application::Run()
{
    while (m_Window->Running())
    {
        Timer::NewFrame();
        
        Update();
        Render();
    }
}
