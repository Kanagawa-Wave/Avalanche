#include "Camera.h"

#include "Engine/Core/Input.h"

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

Camera::Camera(float fov, float aspect, float nearClip, float farClip)
    : m_FOV(fov), m_NearClip(nearClip), m_FarClip(farClip)
{
    m_Projection = glm::perspective(glm::radians(fov), aspect, nearClip, farClip);
    RecalculateView();
}

bool Camera::OnUpdate(float deltaTime)
{
    glm::vec2 mousePos = Input::GetMousePosition();
    glm::vec2 delta = (mousePos - m_LastMousePosition) * 0.002f;
    m_LastMousePosition = mousePos;

    if (!Input::IsMouseButtonPressed(Mouse::ButtonRight))
    {
        Input::SetCursorMode(CursorMode::Normal);
        return false;
    }

    Input::SetCursorMode(CursorMode::Locked);

    bool moved = false;

    constexpr glm::vec3 upDirection(0.0f, 1.0f, 0.0f);
    glm::vec3 rightDirection = glm::cross(m_Forward, upDirection);

    float speed = 10.f;

    // Movement
    if (Input::IsKeyPressed(Key::W))
    {
        m_Position += m_Forward * speed * deltaTime;
        moved = true;
    }
    else if (Input::IsKeyPressed(Key::S))
    {
        m_Position -= m_Forward * speed * deltaTime;
        moved = true;
    }
    if (Input::IsKeyPressed(Key::A))
    {
        m_Position -= rightDirection * speed * deltaTime;
        moved = true;
    }
    else if (Input::IsKeyPressed(Key::D))
    {
        m_Position += rightDirection * speed * deltaTime;
        moved = true;
    }
    if (Input::IsKeyPressed(Key::Q))
    {
        m_Position -= upDirection * speed * deltaTime;
        moved = true;
    }
    else if (Input::IsKeyPressed(Key::E))
    {
        m_Position += upDirection * speed * deltaTime;
        moved = true;
    }

    // Rotation
    if (delta.x != 0.0f || delta.y != 0.0f)
    {
        float pitchDelta = delta.y * 0.3f;
        float yawDelta = delta.x * 0.3f;

        glm::quat q = glm::normalize(glm::cross(glm::angleAxis(-pitchDelta, rightDirection),
                                                glm::angleAxis(-yawDelta, glm::vec3(0.f, 1.0f, 0.0f))));
        m_Forward = glm::rotate(q, m_Forward);

        moved = true;
    }

    if (moved)
    {
        RecalculateView();
    }

    return moved;
}

void Camera::Resize(float aspect)
{
    m_Projection = glm::perspective(glm::radians(m_FOV), aspect, m_NearClip, m_FarClip);
    RecalculateView();
}

void Camera::RecalculateView()
{
    m_View = glm::lookAt(m_Position, m_Position + m_Forward, glm::vec3(0, 1, 0));
}
