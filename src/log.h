//
// Created by Michal Přikryl on 22.12.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once

#include <memory>
#include <spdlog/spdlog.h>

namespace RayTracePlugin {
    class Log {
    public:
        static void Init();
        static void Close();

        static std::shared_ptr<spdlog::logger>& GetLogger() { return m_FP_logger; }

    private:
        static std::shared_ptr<spdlog::logger> m_FP_logger;
    };
}

#define FP_TRACE(fmt, ...)    ::RayTracePlugin::Log::GetLogger()->trace("- [ " fmt " ] -", ##__VA_ARGS__)
#define FP_DEBUG(fmt, ...)    ::RayTracePlugin::Log::GetLogger()->debug("- [ " fmt " ] -", ##__VA_ARGS__)
#define FP_INFO(fmt, ...)     ::RayTracePlugin::Log::GetLogger()->info("- [ " fmt " ] -", ##__VA_ARGS__)
#define FP_WARN(fmt, ...)     ::RayTracePlugin::Log::GetLogger()->warn("- [ " fmt " ] -", ##__VA_ARGS__)
#define FP_ERROR(fmt, ...)    ::RayTracePlugin::Log::GetLogger()->error("- [ " fmt " ] -", ##__VA_ARGS__)
#define FP_CRITICAL(fmt, ...) ::RayTracePlugin::Log::GetLogger()->critical("- [ " fmt " ] -", ##__VA_ARGS__)
