#include <atomic>
#include <cstdio>
#include <kc/log.hpp>

namespace kc {
namespace {

std::atomic<Level> current_level{Level::Warn};

const char* level_name(Level level) {
    switch (level) {
        case Level::Debug:
            return "debug";
        case Level::Info:
            return "info";
        case Level::Warn:
            return "warn";
        case Level::Error:
            return "error";
    }
    return "?";
}

void write(Level level, const std::string& message) {
    if (level < current_level) return;
    std::fprintf(stderr, "[kc] %s: %s\n", level_name(level), message.c_str());
}

}  // namespace

void set_log_level(Level level) { current_level = level; }

void log_debug(const std::string& message) { write(Level::Debug, message); }

void log_info(const std::string& message) { write(Level::Info, message); }

void log_warn(const std::string& message) { write(Level::Warn, message); }

void log_error(const std::string& message) { write(Level::Error, message); }

}  // namespace kc
