#pragma once
#include <format>
#include <memory>
#include <string_view>

namespace spdlog
{
    class logger;
}

namespace Core
{
    class Logger
    {
    public:
        static void Initialize();
        static void Shutdown();

        template<typename... Args>
        static void Trace(
            std::format_string<Args...> fmt,
            Args&&... args)
        {
            Write(
                Level::Trace,
                std::format(fmt, std::forward<Args>(args)...));
        }

        template<typename... Args>
        static void Info(
            std::format_string<Args...> fmt,
            Args&&... args)
        {
            Write(
                Level::Info,
                std::format(fmt, std::forward<Args>(args)...));
        }

        template<typename... Args>
        static void Warn(
            std::format_string<Args...> fmt,
            Args&&... args)
        {
            Write(
                Level::Warn,
                std::format(fmt, std::forward<Args>(args)...));
        }

        template<typename... Args>
        static void Error(
            std::format_string<Args...> fmt,
            Args&&... args)
        {
            Write(
                Level::Error,
                std::format(fmt, std::forward<Args>(args)...));
        }

    private:
        enum class Level
        {
            Trace,
            Info,
            Warn,
            Error
        };

        static void Write(
            Level level,
            std::string_view message);

        static std::shared_ptr<spdlog::logger> s_logger;
    };
}