#include "Window.h"

#include <GLFW/glfw3.h>

Window::Window(uint32_t width, uint32_t height, const std::string& name)
    : m_WindowData(width, height, name)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    m_Window = glfwCreateWindow((int)width, (int)height, name.c_str(), nullptr, nullptr);

    glfwSetWindowUserPointer(m_Window, &m_WindowData);
    
    glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
    {
        WindowData* windowData = reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
        windowData->m_Width = width;
        windowData->m_Height = height;
        windowData->m_Resized = true; 
    });
}

Window::~Window()
{
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

void Window::Init()
{
    m_Swapchain = std::make_unique<Swapchain>(m_WindowData.m_Width, m_WindowData.m_Height);
}

void Window::Destory()
{
    m_Swapchain.reset();
}

bool Window::Running() const
{
    return !glfwWindowShouldClose(m_Window);
}

void Window::PollEvents() const
{
    glfwPollEvents();
}
