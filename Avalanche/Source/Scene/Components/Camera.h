#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
    Camera() = default;
    Camera(uint32_t width, uint32_t height, float fov, float nearClip, float farClip);

    glm::mat4 GetViewProjection() const { return m_Projection * m_View; }
    glm::mat4 GetView() const { return m_View; }
    glm::mat4 GetProjection() const { return m_Projection; }
    glm::vec3 GetPosition() const { return m_Position; }
    
    bool OnUpdate(float deltaTime);
    void OnResize(uint32_t width, uint32_t height);
private:
    void RecalculateView();
    
    glm::vec2 m_LastMousePosition{0};
    glm::vec3 m_Position{0, 0, 6}, m_Forward{0, 0, -1};
    glm::mat4 m_View{1.0}, m_Projection{1.0};

    uint32_t m_Width = 0, m_Height = 0;
    float m_FOV = 0.f, m_NearClip = 0.f, m_FarClip = 0.f;
};
