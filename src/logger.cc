#include "logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace mcpe_viz {

    auto create_console_sink()
    {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_pattern("[%^%7l%$] %v");
#ifdef _WIN32
        console_sink->set_color(spdlog::level::info, console_sink->WHITE);
#else
        console_sink->set_color(spdlog::level::info, console_sink->white);
#endif
        return console_sink;
    }

    void setup_logger_stage_1()
    {
        spdlog::set_default_logger(std::make_shared<spdlog::logger>("stage_1", create_console_sink()));
    }

    void setup_logger_stage_2(Level consoleLevel)
    {
        auto console_sink = create_console_sink();
        console_sink->set_level(spdlog::level::level_enum(consoleLevel));

        spdlog::sinks_init_list sink_list = { console_sink };

        spdlog::set_default_logger(std::make_shared<spdlog::logger>("stage_2", sink_list));
        // set global log level
        spdlog::set_level(spdlog::level::level_enum(consoleLevel));
    }

}
