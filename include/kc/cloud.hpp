#pragma once

#include <cstdint>
#include <kc/frame.hpp>
#include <vector>

namespace kc {

enum class ColorMode { DepthGradient, None };

struct Point {
    float x, y, z;
    uint8_t r, g, b;
};

struct Cloud {
    std::vector<Point> points;
    uint64_t timestamp = 0;
};

Cloud build_cloud(const Frame& frame,
                  ColorMode color = ColorMode::DepthGradient);

}  // namespace kc
