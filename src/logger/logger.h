#ifndef LOGGER_H__
#define LOGGER_H__

#include <fmt/core.h>
enum class LoggerLvl {
    STATUS  = 3,
    WARNING = 2,
    ERROR   = 1,
};
template<typename... Ts>
void log(LoggerLvl lvl, char const * file, int line,
         fmt::format_string<Ts...> format, Ts... args) {
    vlog(lvl, file, line, format, fmt::make_format_args(args...));
}
void vlog(LoggerLvl lvl, char const * file, int line, fmt::string_view format,
          fmt::format_args args);

#ifdef DEBUG_
#define LOG(lvl, format, ...) \
    log(lvl, __FILE__, __LINE__, format, ##__VA_ARGS__)
#else
#define LOG(lvl, format, ...) [] {}()
#endif

#endif