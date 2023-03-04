#include "Window.h"

#include <GLFW/glfw3.h>

Window::Window(uint32_t width, uint32_t height, const std::string& name)
    : m_Width(width), m_Height(height), m_Name(name)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    m_Window = glfwCreateWindow((int)width, (int)height, name.c_str(), nullptr, nullptr);
}

Window::~Window()
{
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

bool Window::Running() const
{
    return !glfwWindowShouldClose(m_Window);
}

void Window::PollEvents() const
{
    glfwPollEvents();
}

GLFWwindow* Window::GetGLFWWindow() const
{
    return m_Window;
}

std::pair<uint32_t, uint32_t> Window::GetExtent() const
{
    return {m_Width, m_Height};
}

uint32_t Window::GetWidth() const
{
    return m_Width;
}

uint32_t Window::GetHeight() const
{
    return m_Height;
}
