#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>


class Log
{
public:
    static void Init();

    static inline std::shared_ptr<spdlog::logger>& GetCoreLogger()
    {
        return s_CoreLogger;
    }

    static inline std::shared_ptr<spdlog::logger>& GetClientLogger()
    {
        return s_ClientLogger;
    }

    static inline std::shared_ptr<spdlog::logger>& GetFileLogger()
    {
        return s_FileLogger;
    }

private:
    static std::shared_ptr<spdlog::logger> s_CoreLogger;
    static std::shared_ptr<spdlog::logger> s_ClientLogger;
    static std::shared_ptr<spdlog::logger> s_FileLogger;
};


// Core Log Macros
#define LOG_F(...) {::Log::GetCoreLogger()->fatal(__VA_ARGS__);\
	::Log::GetFileLogger()->fatal(__VA_ARGS__);}
#define LOG_E(...) {::Log::GetCoreLogger()->error(__VA_ARGS__);\
	::Log::GetFileLogger()->error(__VA_ARGS__);}
#define LOG_W(...) {::Log::GetCoreLogger()->warn(__VA_ARGS__);\
	::Log::GetFileLogger()->warn(__VA_ARGS__);}
#define LOG_I(...) {::Log::GetCoreLogger()->info(__VA_ARGS__);\
	::Log::GetFileLogger()->info(__VA_ARGS__);}
#define LOG_T(...) {::Log::GetCoreLogger()->trace(__VA_ARGS__);\
	::Log::GetFileLogger()->trace(__VA_ARGS__);}

#ifdef _DEBUG
#define ASSERT(x, ...) { if(!(x)) { LOG_E("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define CHECK(x)                                                \
do                                                              \
{                                                               \
    vk::Result res = x;                                         \
    if (res != vk::Result::eSuccess)                            \
    {                                                           \
        LOG_E("Vulkan Check Failed: {0}", res);                 \
        __debugbreak();                                         \
    }                                                           \
} while (0)
#else
#define ASSERT(x, ...)
#define CHECK(x)
#endif
#define KT_THROW_EXCEPTION(...) { throw std::runtime_error(__VA_ARGS__);}
