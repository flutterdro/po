#ifndef LOGGER_H__
#define LOGGER_H__

#include <fmt/core.h>
#include <filesystem>

namespace fs = std::filesystem;

namespace tmp {
class source_location {
public:
    static constexpr auto current(const char* file = __builtin_FILE(),
        const char* func = __builtin_FUNCTION(),
        uint_least32_t line = __builtin_LINE(),
#if __has_builtin(__builtin_COLUMN)
        uint_least32_t col = __builtin_COLUMN()) noexcept 
    -> source_location {
#else
          uint_least32_t __col = 0) noexcept 
    -> source_location {
#endif
        source_location sloc;
        sloc.file_ = file;
        sloc.func_ = func;
        sloc.line_ = line;
        sloc.col_ = col;
        return sloc;
    }

    constexpr source_location() noexcept : file_("unknown"),
                                         func_(file_),
                                         line_(0),
                                         col_(0) {}

    constexpr auto line() const noexcept 
        -> uint_least32_t { return line_; }
    constexpr auto column() const noexcept 
        -> uint_least32_t { return col_; }
    constexpr auto file_name() const noexcept 
        -> char const*{ return file_; }
    constexpr auto function_name() const noexcept 
        -> char const* { return func_; }

private:
    const char* file_;
    const char* func_;
    uint_least32_t line_;
    uint_least32_t col_;
};
}

using tmp::source_location;


enum class LogLvl {
    STATUS  = 3,
    WARNING = 2,
    ERROR   = 1,
};
struct LogInfo {

    source_location loc;
    LogLvl lvl;

    constexpr LogInfo(LogLvl lvl, source_location const& loc = source_location::current())
        : lvl(lvl), loc(loc) {}
};
template<typename... Ts>
constexpr void log(LogInfo info, fmt::format_string<Ts...> format, Ts&&... args) {
    vlog(info.lvl, info.loc, format, fmt::make_format_args(args...));
}
void vlog(LogLvl lvl, source_location const& loc, fmt::string_view format,
          fmt::format_args args);

// #ifdef DEBUG_
// #define LOG(lvl, format, ...) \
//     log(lvl, __FILE__, __LINE__, format, ##__VA_ARGS__)
// #else
// #define LOG(lvl, format, ...) [] {}()
// #endif

#endif


