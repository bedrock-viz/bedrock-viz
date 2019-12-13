#include "logger.h"

#include <cstdarg>
#include <cstdlib>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace mcpe_viz {
    Logger::Logger() {

        this->logLevelMask = kLogInfo;
        this->fpStderr = nullptr;
        this->fpStdout = nullptr;
        this->doFlushFlag = false;
    }

    void Logger::setFlush(bool f) { doFlushFlag = f; }

    void Logger::setLogLevelMask(uint32_t m) { logLevelMask = m; }

    void Logger::setStdout(FILE * fp) { fpStdout = fp; }

    void Logger::setStderr(FILE * fp) { fpStderr = fp; }

    int32_t Logger::msg(uint32_t levelMask, const char * fmt, ...) {
        // check if we care about this message
        if ((levelMask & logLevelMask) || (levelMask & kLogFatalError)) {
            // we care
        } else {
            // we don't care
            return -1;
        }

        // todobig - be more clever about logging - if very important or interesting, we might want to print to log AND to stderr
        FILE *fp = fpStdout;

        if (fp == nullptr) {
            // todobig - output to screen?
            return -1;
        }

        if (levelMask & kLogFatalError) { fprintf(fp, "** FATAL ERROR: "); }
        else if (levelMask & kLogError) { fprintf(fp, "ERROR: "); }
        else if (levelMask & kLogWarning) { fprintf(fp, "WARNING: "); }
//        else if (levelMask & kLogInfo) {} // fprintf(fp,"INFO: "); }
//        else {} // fprintf(fp,"UNKNOWN: "); }

        va_list argptr;
        va_start(argptr, fmt);
        vfprintf(fp, fmt, argptr);
        va_end(argptr);

        if (levelMask & kLogFatalError) {
            fprintf(fp, "** Exiting on FATAL ERROR\n");
            fflush(fp);
            exit(-1);
        }

        if (doFlushFlag) {
            fflush(fp);
        }

        return 0;
    }

    auto create_console_sink()
    {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_pattern("%^%v%$");
#ifdef _WIN32
        console_sink->set_color(spdlog::level::info, console_sink->WHITE);
#else
        sink->set_color(spdlog::level::info, sink->white);
#endif
        return console_sink;
    }

    void setup_logger_stage_1()
    {
        spdlog::set_default_logger(std::make_shared<spdlog::logger>("stage_1", create_console_sink()));
    }

    void setup_logger_stage_2(const std::filesystem::path& outpath, Level consoleLevel, Level fileLevel)
    {
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(outpath.generic_string());
        // file_sink always record all log
        file_sink->set_level(spdlog::level::level_enum(fileLevel));
        file_sink->set_pattern("[%Y-%m-%d %T.%e][%l] %v");
        auto console_sink = create_console_sink();
        
        console_sink->set_level(spdlog::level::level_enum(consoleLevel));

        spdlog::sinks_init_list sink_list = { file_sink, console_sink };

        spdlog::set_default_logger(std::make_shared<spdlog::logger>("stage_2", sink_list));
        // set global log level
        // TODO we can't assume int(debug) < int(info) < info(warn)
        spdlog::set_level(spdlog::level::level_enum(consoleLevel > fileLevel ? fileLevel : consoleLevel));
    }

    Logger logger;
    Logger slogger;
}
