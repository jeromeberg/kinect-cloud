#include <libfreenect_sync.h>

#include <kc/device.hpp>
#include <kc/log.hpp>
#include <string>

namespace kc {
namespace {

constexpr int DEVICE_INDEX = 0;
constexpr uint32_t DEPTH_WIDTH = 640;
constexpr uint32_t DEPTH_HEIGHT = 480;
constexpr uint64_t DEVICE_TICKS_PER_SECOND = 60000000;
constexpr uint64_t MICROSECONDS_PER_SECOND = 1000000;

uint64_t ticks_to_us(uint64_t ticks) {
    return ticks * MICROSECONDS_PER_SECOND / DEVICE_TICKS_PER_SECOND;
}

bool grab_depth(void** depth, uint32_t* ticks) {
    return freenect_sync_get_depth(depth, ticks, DEVICE_INDEX,
                                   FREENECT_DEPTH_MM) == 0;
}

void fill_frame(Frame& frame, const void* depth, uint64_t timestamp) {
    const uint16_t* pixels = static_cast<const uint16_t*>(depth);
    frame.timestamp = timestamp;
    frame.width = DEPTH_WIDTH;
    frame.height = DEPTH_HEIGHT;
    frame.depth.assign(pixels, pixels + DEPTH_WIDTH * DEPTH_HEIGHT);
}

}  // namespace

Device::~Device() { close(); }

bool Device::open() {
    if (streaming_) return true;

    // first frame checks if device is reachable
    void* depth = nullptr;
    uint32_t ticks = 0;
    if (!grab_depth(&depth, &ticks)) {
        log_error("cannot open kinect device");
        return false;
    }

    has_base_ = false;
    streaming_ = true;
    log_info("kinect device opened");
    log_debug("depth stream: device " + std::to_string(DEVICE_INDEX) + ", " +
              std::to_string(DEPTH_WIDTH) + "x" + std::to_string(DEPTH_HEIGHT) +
              ", millimeters, " + std::to_string(DEVICE_TICKS_PER_SECOND) +
              " ticks per second");
    return true;
}

void Device::stop() {
    if (!streaming_.exchange(false)) return;
    freenect_sync_stop();
    log_info("kinect device stopped");
}

void Device::close() { stop(); }

bool Device::next(Frame& frame) {
    if (!streaming_) return false;

    void* depth = nullptr;
    uint32_t ticks = 0;
    if (!grab_depth(&depth, &ticks)) {
        log_warn("depth capture failed");
        stop();
        return false;
    }
    if (!streaming_) return false;

    fill_frame(frame, depth, rebase_timestamp(ticks));
    return true;
}

uint64_t Device::rebase_timestamp(uint32_t ticks) {
    uint32_t delta_ticks = 0;
    if (!has_base_) {
        has_base_ = true;
        elapsed_ticks_ = 0;
    } else {
        delta_ticks = static_cast<uint32_t>(ticks - last_ticks_);
        elapsed_ticks_ += delta_ticks;
    }
    last_ticks_ = ticks;

    uint64_t timestamp = ticks_to_us(elapsed_ticks_);
    return timestamp;
}

}  // namespace kc
