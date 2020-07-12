#pragma once

#include <filesystem>
#include <spdlog/spdlog.h>

namespace mcpe_viz {
    namespace log
    {
        using spdlog::trace;
        using spdlog::debug;
        using spdlog::info;
        using spdlog::warn;
        using spdlog::error;
    }
    

    enum class Level {
        Trace = spdlog::level::trace,
        Debug = spdlog::level::debug,
        Info = spdlog::level::info,
        Warn = spdlog::level::warn,
        Error = spdlog::level::err
    };


    // setup a console-only logger since we don't know the output path
    void setup_logger_stage_1();

    // setup both console logger and file logger
    void setup_logger_stage_2(const std::filesystem::path& outdir, Level consoleLevel, Level fileLevel);
}
