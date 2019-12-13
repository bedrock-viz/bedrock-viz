#pragma once

#include <cstdio>
#include <cstdint>
#include <filesystem>
#include <spdlog/spdlog.h>

namespace mcpe_viz {

    enum LogType : uint32_t {
        // todobig - be more clever about this
        kLogInfo1 = 0x0010,

        kLogInfo = 0x0010,
        kLogWarning = 0x0020,
        kLogError = 0x0040,
        kLogFatalError = 0x0080,
        kLogDebug = 0x1000,
        kLogAll = 0xffff
    };

    const uint32_t kLogVerbose = kLogAll ^ (kLogDebug);
    const uint32_t kLogDefault = kLogVerbose;
    const uint32_t kLogQuiet = ( kLogWarning | kLogError | kLogFatalError );


    class Logger {
    public:
        uint32_t logLevelMask;
        FILE *fpStdout;
        FILE *fpStderr;
        bool doFlushFlag;

        Logger();

        void setFlush(bool f);

        void setLogLevelMask(uint32_t m);

        void setStdout(FILE *fp);

        void setStderr(FILE *fp);

        #if defined(__GNUC__) || defined(__clang__)
        __attribute__((format(printf, 3, 4)))
        #endif
        int32_t msg(uint32_t levelMask, const char *fmt, ...);

    };

    extern Logger logger;
    extern Logger slogger;

    using spdlog::debug;
    using spdlog::info;
    using spdlog::warn;
    using spdlog::error;

    enum class Level {
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
