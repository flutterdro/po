#include "logger.h"

void vlog(LogLvl lvl, source_location const& loc, fmt::string_view format,
          fmt::format_args args) {
    fs::path file_name{loc.file_name()};
    switch (lvl) {
    case LogLvl::STATUS:
        fmt::print("[STATUS] ");
        fmt::vprint(format, args);
        break;
    case LogLvl::WARNING:
        fmt::print("[WARNING] in {}:{}:{} in the function: {}\n   ", 
            file_name.filename().c_str(), loc.line(), loc.column(), loc.function_name());
        fmt::vprint(format, args);
        break;
    case LogLvl::ERROR:
        fmt::print(stderr, "[ERROR] in {}:{}:{} in the function: {}\n   ", 
            file_name.filename().c_str(), loc.line(), loc.column(), loc.function_name());
        fmt::vprint(stderr, format, args);
        break;
    }
}