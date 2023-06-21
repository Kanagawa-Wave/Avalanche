#include "Timer.h"

#include <GLFW/glfw3.h>

float Timer::m_Time = 0.f, Timer::m_DeltaTime = 0.f, Timer::m_LastFrameTime = 0.f;

void Timer::Init()
{
}

void Timer::NewFrame()
{
    const float time = (float)glfwGetTime();
    m_DeltaTime = time - m_LastFrameTime;
    m_LastFrameTime = time;
}

float Timer::GetDeltaTime()
{
    return m_DeltaTime;
}

float Timer::GetDeltaTimeInMillliseconds()
{
    return m_DeltaTime * 1000.f;
}
