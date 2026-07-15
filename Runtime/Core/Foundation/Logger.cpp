#include "pch.h"
#include "Logger.h"
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Core
{
    std::shared_ptr<spdlog::logger> Logger::s_logger;

    void Logger::Initialize()
    {
        if (s_logger)
            return;

        auto consoleSink =
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

        auto fileSink =
            std::make_shared<spdlog::sinks::basic_file_sink_mt>(
                "Engine.log",
                true); //true면 새로 log 파일 만들기. false 면 log 파일뒤에 이어쓰기.

        std::vector<spdlog::sink_ptr> sinks
        {
            consoleSink,
            fileSink
        };

        s_logger = std::make_shared<spdlog::logger>(
            "Engine",
            sinks.begin(),
            sinks.end());

        s_logger->set_level(spdlog::level::trace);

        s_logger->set_pattern(
            "[%H:%M:%S.%e] [%^%l%$] %v");

        spdlog::set_default_logger(s_logger);
    }

    void Logger::Shutdown()
    {
        s_logger.reset();
        spdlog::shutdown();
    }

    void Logger::Write(
        Level level,
        std::string_view message)
    {
        if (!s_logger)
            return;

        switch (level)
        {
        case Level::Trace:
            s_logger->trace(message);
            break;

        case Level::Info:
            s_logger->info(message);
            break;

        case Level::Warn:
            s_logger->warn(message);
            break;

        case Level::Error:
            s_logger->error(message);
            break;
        }
    }
}