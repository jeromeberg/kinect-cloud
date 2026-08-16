#pragma once

#include <string>

namespace kc {

enum class Level { Debug, Info, Warn, Error };

void set_log_level(Level level);

void log_debug(const std::string& message);
void log_info(const std::string& message);
void log_warn(const std::string& message);
void log_error(const std::string& message);

}  // namespace kc
