#pragma once

class Timer
{
public:
    static void Init();

    static void NewFrame();

    static float GetDeltaTime();
    static float GetDeltaTimeInMillliseconds();

private:
    static float m_Time, m_LastFrameTime, m_DeltaTime;
};
