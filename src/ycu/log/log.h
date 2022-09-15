#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <memory>

#define YCU_LOG_BEGIN namespace ycu::log {
#define YCU_LOG_END }

YCU_LOG_BEGIN

class Log
{
public:
    static void Init();
    static auto& Get()
    {
        if (!s_Logger)
        {
            throw std::runtime_error("Logger not initialized");
        }
        return s_Logger;
    }
private:
    static std::shared_ptr<spdlog::logger> s_Logger;
};

YCU_LOG_END

#define LOG_INFO(...)       ycu::log::Log::Get()->info(__VA_ARGS__)
#define LOG_WARN(...)       ycu::log::Log::Get()->warn(__VA_ARGS__)
#define LOG_ERROR(...)      ycu::log::Log::Get()->error(__VA_ARGS__)

#define YCU_ENABLE_ASSERTIONS

#ifdef YCU_ENABLE_ASSERTIONS
#define ASSERT(x) \
    if (!(x)) {LOG_ERROR("Assertion failed: "  #x);}
#else
#define ASSERT(x)
#endif