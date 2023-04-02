#pragma once
#include <chrono>

class Timer
{
public:
    static void Init()
    {
        Reset();
    }

    static void Reset()
    {
        m_Start = std::chrono::high_resolution_clock::now();
    }

    static float Elapsed()
    {
        return (float)std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now() - m_Start).count() * 0.001f * 0.001f * 0.001f;
    }

    static float ElapsedMillis()
    {
        return Elapsed() * 1000.0f;
    }

private:
    static std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
};
