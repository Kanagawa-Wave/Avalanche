#pragma once

#include <string>

#include "Engine/Renderer/Vulkan/Swapchain.h"

struct GLFWwindow;

class Window
{
public:
    Window(uint32_t width = 800, uint32_t height = 600, const std::string& name = "Engine");
    ~Window();

    void CreateSwapchain();
    void Destory();

    bool Running() const;
    void PollEvents() const;

    GLFWwindow* GetGLFWWindow() const { return m_Window; }
    Swapchain* GetSwapchain() const { return m_Swapchain.get(); }
    vk::Extent2D GetExtent() const { return {m_WindowData.m_Width, m_WindowData.m_Height}; }
    uint32_t GetWidth() const { return m_WindowData.m_Width; }
    uint32_t GetHeight() const { return m_WindowData.m_Height; }
    float GetAspect() const { return (float)m_WindowData.m_Width / (float)m_WindowData.m_Height; }
    vk::Rect2D GetScissor() const
    {
        vk::Rect2D scissor;
        scissor.setOffset({0, 0})
               .setExtent(GetSwapchain()->GetExtent());
        return scissor;
    }
    vk::Viewport GetViewport() const
    {
        vk::Viewport viewport;
        viewport.setX(0.f)
                .setY((float)GetSwapchain()->GetExtent().height)
                .setWidth((float)GetSwapchain()->GetExtent().width)
                .setHeight(-(float)GetSwapchain()->GetExtent().height)
                .setMinDepth(0.f)
                .setMaxDepth(1.f);
        return viewport;
    }

    bool SwapchainOutdated() const { return m_WindowData.m_Resized; }
    void SwapchainResized() { m_WindowData.m_Resized = false; }
    void RecreateSwapchain(vk::RenderPass renderPass);

private:
    struct WindowData
    {
        uint32_t m_Width = 0, m_Height = 0;
        std::string m_Name;
        bool m_Resized = false;

        WindowData(uint32_t width, uint32_t height, const std::string& name)
            : m_Width(width), m_Height(height), m_Name(name)
        {
        }
    } m_WindowData;

    GLFWwindow* m_Window = nullptr;

    std::unique_ptr<Swapchain> m_Swapchain;
};
