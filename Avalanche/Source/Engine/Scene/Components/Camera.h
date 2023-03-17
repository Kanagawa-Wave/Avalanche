﻿#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
    Camera(float fov, float aspect, float nearClip, float farClip);

    bool OnUpdate(float deltaTime);
private:
    void RecalculateView();
    
    glm::vec2 m_LastMousePosition{0};
    glm::vec3 m_Position{0, 0, 6}, m_Forward{0, 0, -1};
    glm::mat4 m_View{1.0}, m_Projection{1.0};
};
