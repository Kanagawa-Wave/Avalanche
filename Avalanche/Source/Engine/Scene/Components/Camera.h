#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
    Camera(float fov, float aspect, float nearClip, float farClip);

    glm::mat4 GetViewProjection() const { return m_Projection * m_View; }
    glm::mat4 GetView() const { return m_View; }
    glm::mat4 GetProjection() const { return m_Projection; }
    
    bool OnUpdate(float deltaTime);
    void Resize(float aspect);
private:
    void RecalculateView();
    
    glm::vec2 m_LastMousePosition{0};
    glm::vec3 m_Position{0, 0, 6}, m_Forward{0, 0, -1};
    glm::mat4 m_View{1.0}, m_Projection{1.0};

    float m_FOV, m_NearClip, m_FarClip;
};
