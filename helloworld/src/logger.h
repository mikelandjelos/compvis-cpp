#pragma once

#include <fmt/color.h>
#include <fmt/format.h>
#include <string>
#include <string_view>
#include <utility>

namespace logger
{

enum class Level : int
{
    DEBUG = 0,
    INFO = 1,
    WARN = 2,
    ERROR = 3,
    FATAL = 4
};

class Logger
{
  public:
    // Named logger (preferred)
    explicit Logger(std::string name, Level level = Level::INFO, std::string format = "{}");
    // Unnamed logger (kept for compatibility)
    explicit Logger(Level level = Level::INFO, std::string format = "{}");
    ~Logger();

    void set_level(Level l) noexcept;
    Level get_level() const noexcept;

    void set_format(std::string f);
    const std::string& get_format() const noexcept;

    void set_name(std::string n);
    const std::string& get_name() const noexcept;

    template <typename... Args>
    void log(Level l, fmt::format_string<Args...> fmtstr, Args&&... args)
    {
        if (static_cast<int>(l) < static_cast<int>(level_))
            return;
        const std::string body = fmt::format(fmtstr, std::forward<Args>(args)...);
        const std::string final = fmt::format(format_, body);
        emit(l, final);
    }

    template <typename... Args> void debug(fmt::format_string<Args...> s, Args&&... args)
    {
        log(Level::DEBUG, s, std::forward<Args>(args)...);
    }
    template <typename... Args> void info(fmt::format_string<Args...> s, Args&&... args)
    {
        log(Level::INFO, s, std::forward<Args>(args)...);
    }
    template <typename... Args> void warn(fmt::format_string<Args...> s, Args&&... args)
    {
        log(Level::WARN, s, std::forward<Args>(args)...);
    }
    template <typename... Args> void error(fmt::format_string<Args...> s, Args&&... args)
    {
        log(Level::ERROR, s, std::forward<Args>(args)...);
    }
    template <typename... Args> void fatal(fmt::format_string<Args...> s, Args&&... args)
    {
        log(Level::FATAL, s, std::forward<Args>(args)...);
    }

  private:
    Level level_;
    std::string name_;
    std::string format_;

    void emit(Level l, std::string_view text) const;
};

} // namespace logger
