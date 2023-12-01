#ifndef LOGGER_H__
#define LOGGER_H__

#include <fmt/core.h>
#include <filesystem>
#include <source_location>


namespace fs = std::filesystem;

enum class LogLvl {
    STATUS  = 3,
    WARNING = 2,
    ERROR   = 1,
};
struct LogInfo {

    LogLvl lvl;
    std::source_location loc;

    constexpr LogInfo(LogLvl lvl, std::source_location const& loc = std::source_location::current())
        : lvl{lvl}, loc{loc} {}
};
template<typename... Ts>
constexpr void log(LogInfo info, fmt::format_string<Ts...> format, Ts&&... args) {
    vlog(info.lvl, info.loc, format, fmt::make_format_args(args...));
}
void vlog(LogLvl lvl, std::source_location const& loc, fmt::string_view format,
          fmt::format_args args);

// #ifdef DEBUG_
// #define LOG(lvl, format, ...) \
//     log(lvl, __FILE__, __LINE__, format, ##__VA_ARGS__)
// #else
// #define LOG(lvl, format, ...) [] {}()
// #endif

template<typename T, size_t... indices, typename... Ts>
[[nodiscard]] static inline constexpr auto make_array_impl(std::index_sequence<indices...>, Ts&&... args) noexcept 
    -> std::array<T, sizeof...(indices)> {
    return { (static_cast<void>(indices), T{std::forward<Ts>(args)...})... };
}
template<typename Container>
concept InitListContainer = requires(Container&&, std::initializer_list<typename Container::value_type> list) {
    Container(list);
};

template<typename Container, size_t... indices, typename... Ts>
    requires InitListContainer<Container>
[[nodiscard]] static inline constexpr auto make_array_impl(std::index_sequence<indices...>, Ts&&... args) noexcept 
    -> std::array<Container, sizeof...(indices)> {
    return { (static_cast<void>(indices), Container(std::forward<Ts>(args)...))... };
}

template<typename T, size_t N, typename... Ts>
[[nodiscard]] static inline constexpr auto make_array(Ts&&... args) noexcept
    -> std::array<T, N> {
    return make_array_impl<T>(std::make_index_sequence<N>(), std::forward<Ts>(args)...);
}



#endif


