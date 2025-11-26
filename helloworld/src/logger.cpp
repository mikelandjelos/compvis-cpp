#include "logger.h"

#include <chrono>
#include <cstdio>
#include <ctime>
#include <fmt/chrono.h>

namespace logger
{

static const char* to_string(Level l)
{
    switch (l)
    {
    case Level::DEBUG:
        return "DEBUG";
    case Level::INFO:
        return "INFO";
    case Level::WARN:
        return "WARN";
    case Level::ERROR:
        return "ERROR";
    case Level::FATAL:
        return "FATAL";
    }
    return "?";
}

static fmt::text_style style_for(Level l)
{
    using fmt::color;
    using fmt::emphasis;
    switch (l)
    {
    case Level::DEBUG:
        return fg(color::gray);
    case Level::INFO:
        return fg(color::white);
    case Level::WARN:
        return fg(color::yellow) | emphasis::bold;
    case Level::ERROR:
        return fg(color::red) | emphasis::bold;
    case Level::FATAL:
        return fg(color::crimson) | emphasis::bold;
    }
    return {};
}

Logger::Logger(std::string name, Level level, std::string format)
    : level_(level), name_(std::move(name)), format_(std::move(format))
{
}

Logger::Logger(Level level, std::string format) : level_(level), name_(), format_(std::move(format))
{
}

Logger::~Logger() = default;

void Logger::set_level(Level l) noexcept
{
    level_ = l;
}
Level Logger::get_level() const noexcept
{
    return level_;
}

void Logger::set_format(std::string f)
{
    format_ = std::move(f);
}
const std::string& Logger::get_format() const noexcept
{
    return format_;
}

void Logger::set_name(std::string n)
{
    name_ = std::move(n);
}
const std::string& Logger::get_name() const noexcept
{
    return name_;
}

void Logger::emit(Level l, std::string_view text) const
{
    FILE* out = (l == Level::WARN || l == Level::ERROR || l == Level::FATAL) ? stderr : stdout;
    const auto style = style_for(l);

    // Timestamp: local time HH:MM:SS.mmm
    const auto now = std::chrono::system_clock::now();
    const auto t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) %
                    std::chrono::seconds(1);

    if (!name_.empty())
    {
        fmt::print(out, style, "[{}] [{}] [{:%H:%M:%S}.{:03d}] {}\n", to_string(l), name_, tm,
                   static_cast<int>(ms.count()), text);
    }
    else
    {
        fmt::print(out, style, "[{}] [{:%H:%M:%S}.{:03d}] {}\n", to_string(l), tm,
                   static_cast<int>(ms.count()), text);
    }
}

} // namespace logger
