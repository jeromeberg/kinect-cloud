#include <algorithm>
#include <cstddef>
#include <kc/cloud.hpp>
#include <kc/log.hpp>
#include <string>

namespace kc {
namespace {

constexpr float FX = 594.21f;
constexpr float FY = 591.04f;
constexpr float CX = 339.5f;
constexpr float CY = 242.7f;

constexpr float MILLIMETERS_PER_METER = 1000.0f;
constexpr float GRADIENT_NEAR = 0.5f;
constexpr float GRADIENT_FAR = 5.0f;

uint8_t to_byte(float value) {
    return static_cast<uint8_t>(std::min(std::max(value, 0.0f), 1.0f) * 255.0f);
}

void set_gradient_color(Point& point) {
    float span = (point.z - GRADIENT_NEAR) / (GRADIENT_FAR - GRADIENT_NEAR);
    span = std::min(std::max(span, 0.0f), 1.0f);
    if (span < 0.5f) {
        point.r = to_byte(1.0f - 2.0f * span);
        point.g = to_byte(2.0f * span);
        point.b = 0;
    } else {
        point.r = 0;
        point.g = to_byte(2.0f - 2.0f * span);
        point.b = to_byte(2.0f * span - 1.0f);
    }
}

Point make_point(uint32_t x, uint32_t y, uint16_t depth, ColorMode color) {
    Point point;
    point.z = static_cast<float>(depth) / MILLIMETERS_PER_METER;
    point.x = (static_cast<float>(x) - CX) * point.z / FX;
    point.y = (static_cast<float>(y) - CY) * point.z / FY;
    if (color == ColorMode::DepthGradient) {
        set_gradient_color(point);
    } else {
        point.r = point.g = point.b = 255;
    }
    return point;
}

bool has_full_depth(const Frame& frame) {
    return frame.depth.size() >=
           static_cast<size_t>(frame.width) * frame.height;
}

}  // namespace

Cloud build_cloud(const Frame& frame, ColorMode color) {
    Cloud cloud;
    cloud.timestamp = frame.timestamp;
    if (!has_full_depth(frame)) {
        log_warn("frame depth buffer smaller than its size, cloud is empty");
        return cloud;
    }

    cloud.points.reserve(frame.depth.size());
    for (uint32_t y = 0; y < frame.height; ++y) {
        for (uint32_t x = 0; x < frame.width; ++x) {
            uint16_t depth = frame.depth[y * frame.width + x];
            if (depth != 0)
                cloud.points.push_back(make_point(x, y, depth, color));
        }
    }

    return cloud;
}

}  // namespace kc
