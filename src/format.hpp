#pragma once

#include <cstdint>

namespace kc {

constexpr char FILE_MAGIC[4] = {'K', 'C', 'J', 'B'};
constexpr uint32_t FILE_VERSION = 1;
constexpr uint32_t FILE_DEPTH_WIDTH = 640;
constexpr uint32_t FILE_DEPTH_HEIGHT = 480;

}  // namespace kc
