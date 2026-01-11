//
// Created by Michal Přikryl on 22.12.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#include "log.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/cfg/env.h>

#if defined(_WIN32)
#include <windows.h>
#endif

namespace RayTracePlugin {
    std::shared_ptr<spdlog::logger> Log::m_FP_logger;

    void Log::Init() {
#if defined(_WIN32)
        HANDLE hOut = GetStdHandle(STD_ERROR_HANDLE);
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode))
            SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif

        std::vector<spdlog::sink_ptr> sinks;

        auto color_sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
        color_sink->set_pattern("%^[%T.%e] %n: %v%$");

        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("RayTrace.log", true);
        file_sink->set_pattern("[%T.%e] [%^%l%$] %n: %v");

        sinks.emplace_back(color_sink);
        sinks.emplace_back(file_sink);

        m_FP_logger = std::make_shared<spdlog::logger>("RayTrace", sinks.begin(), sinks.end());
        register_logger(m_FP_logger);
        m_FP_logger->set_level(spdlog::level::trace);
        m_FP_logger->flush_on(spdlog::level::info);

        spdlog::cfg::load_env_levels();
    }

    void Log::Close() {
        spdlog::drop("RayTrace");
        m_FP_logger.reset();
    }
}
