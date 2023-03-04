#pragma once

#include <string>

struct GLFWwindow;

class Window
{
public:
    Window(uint32_t width = 800, uint32_t height = 600, const std::string& name = "Engine");
    ~Window();

    bool Running() const;
    void PollEvents() const;
    GLFWwindow* GetGLFWWindow() const;
    std::pair<uint32_t, uint32_t> GetExtent() const;
    uint32_t GetWidth() const;
    uint32_t GetHeight() const;

private:
    uint32_t m_Width = 0, m_Height = 0;
    std::string m_Name;

    GLFWwindow* m_Window = nullptr;
};
