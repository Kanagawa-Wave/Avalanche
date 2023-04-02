#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
std::shared_ptr<spdlog::logger> Log::s_ClientLogger;
std::shared_ptr<spdlog::logger> Log::s_FileLogger;

void Log::Init() {

    time_t t = time(&t);
    auto t_str = std::to_string(t);
    const auto log_dir = "Logs/Log_" + t_str + ".txt";

    spdlog::set_pattern("%^[%T] %n: %v%$");
    s_CoreLogger = spdlog::stdout_color_mt("CORE");
    s_FileLogger = spdlog::basic_logger_mt(
            "CORE_LOGGER",
            log_dir);
    s_CoreLogger->set_level(spdlog::level::trace);
    s_FileLogger->set_level(spdlog::level::trace);
    s_ClientLogger = spdlog::stdout_color_mt("CLIENT");
    s_ClientLogger->set_level(spdlog::level::trace);
}