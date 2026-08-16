#pragma once

#include <cstdint>
#include <vector>

namespace kc {

struct Frame {
    uint64_t timestamp = 0;
    uint32_t width = 0;
    uint32_t height = 0;
    std::vector<uint16_t> depth;
};

}  // namespace kc
