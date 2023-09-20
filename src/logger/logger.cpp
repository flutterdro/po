#include "logger.h"

void vlog(LoggerLvl lvl, char const * file, int line, fmt::string_view format,
          fmt::format_args args) {
    switch (lvl) {
    case LoggerLvl::STATUS:
        fmt::print("[STATUS]");
        break;
    case LoggerLvl::WARNING:
        fmt::print("[WARNING]");
        break;
    case LoggerLvl::ERROR:
        fmt::print("[ERROR]");
        break;
    }
    fmt::print(" In the file: {}; At the line: {}\n  ", file, line);
    fmt::vprint(format, args);
}