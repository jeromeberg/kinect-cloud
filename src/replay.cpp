#include <kc/log.hpp>
#include <kc/replay.hpp>
#include <thread>
#include <vector>

#include "format.hpp"

namespace kc {

Replay::~Replay() { close(); }

bool Replay::open(const std::string& path, bool realtime) {
    close();

    file_ = std::fopen(path.c_str(), "rb");
    if (!file_) {
        log_error("cannot open file: " + path);
        return false;
    }

    if (!read_header()) {
        log_error("invalid file: " + path);
        close();
        return false;
    }

    realtime_ = realtime;
    has_start_ = false;
    log_info("file opened: " + path);
    return true;
}

bool Replay::read_header() {
    char magic[4] = {};
    uint32_t version = 0;
    uint8_t reserved[4] = {};

    if (std::fread(magic, 1, sizeof(magic), file_) != sizeof(magic))
        return false;
    for (size_t i = 0; i < sizeof(magic); ++i)
        if (magic[i] != FILE_MAGIC[i]) return false;

    if (std::fread(&version, sizeof(version), 1, file_) != 1) return false;
    if (version != FILE_VERSION) return false;

    if (std::fread(&width_, sizeof(width_), 1, file_) != 1) return false;
    if (std::fread(&height_, sizeof(height_), 1, file_) != 1) return false;
    if (std::fread(reserved, 1, sizeof(reserved), file_) != sizeof(reserved))
        return false;

    return width_ > 0 && height_ > 0;
}

bool Replay::read_record(Frame& frame) {
    uint64_t timestamp = 0;
    size_t ts_bytes = std::fread(&timestamp, 1, sizeof(timestamp), file_);
    if (ts_bytes == 0) return false;
    if (ts_bytes != sizeof(timestamp)) {
        log_warn("bad timestamp");
        return false;
    }

    size_t sample_count = static_cast<size_t>(width_) * height_;
    std::vector<uint16_t> depth(sample_count);
    size_t depth_bytes =
        std::fread(depth.data(), 1, sample_count * sizeof(uint16_t), file_);
    if (depth_bytes != sample_count * sizeof(uint16_t)) {
        log_warn("bad depth data");
        return false;
    }

    frame.timestamp = timestamp;
    frame.width = width_;
    frame.height = height_;
    frame.depth = std::move(depth);
    return true;
}

void Replay::pace(uint64_t timestamp) {
    if (!has_start_) {
        has_start_ = true;
        start_time_ = std::chrono::steady_clock::now();
        return;
    }
    std::this_thread::sleep_until(start_time_ +
                                  std::chrono::microseconds(timestamp));
}

bool Replay::next(Frame& frame) {
    if (!file_) return false;

    if (!read_record(frame)) {
        close();
        return false;
    }

    if (realtime_) pace(frame.timestamp);
    return true;
}

void Replay::close() {
    if (!file_) return;
    std::fclose(file_);
    file_ = nullptr;
}

}  // namespace kc
