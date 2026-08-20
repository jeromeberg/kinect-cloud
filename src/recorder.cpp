#include <kc/log.hpp>
#include <kc/recorder.hpp>

#include "format.hpp"

namespace kc {

Recorder::~Recorder() { close(); }

bool Recorder::open(const std::string& path) {
    close();

    file_ = std::fopen(path.c_str(), "wb");
    if (!file_) {
        log_error("cannot create file: " + path);
        return false;
    }

    if (!write_header()) {
        log_error("failed to write header: " + path);
        close();
        return false;
    }

    has_base_ = false;
    log_info("recording at: " + path);
    return true;
}

bool Recorder::write_header() {
    uint32_t version = FILE_VERSION;
    uint32_t width = FILE_DEPTH_WIDTH;
    uint32_t height = FILE_DEPTH_HEIGHT;
    uint8_t reserved[4] = {};

    bool ok = std::fwrite(FILE_MAGIC, 1, sizeof(FILE_MAGIC), file_) ==
              sizeof(FILE_MAGIC);
    ok = ok && std::fwrite(&version, sizeof(version), 1, file_) == 1;
    ok = ok && std::fwrite(&width, sizeof(width), 1, file_) == 1;
    ok = ok && std::fwrite(&height, sizeof(height), 1, file_) == 1;
    ok = ok &&
         std::fwrite(reserved, 1, sizeof(reserved), file_) == sizeof(reserved);
    return ok;
}

bool Recorder::write(const Frame& frame) {
    if (!file_) return false;

    if (frame.width != FILE_DEPTH_WIDTH || frame.height != FILE_DEPTH_HEIGHT) {
        log_error("bad frame size");
        return false;
    }

    size_t sample_count = static_cast<size_t>(frame.width) * frame.height;
    if (frame.depth.size() < sample_count) {
        log_error("bad frame depth buffer");
        return false;
    }

    if (!has_base_) {
        has_base_ = true;
        base_timestamp_ = frame.timestamp;
    }
    uint64_t timestamp = frame.timestamp - base_timestamp_;

    bool ok = std::fwrite(&timestamp, sizeof(timestamp), 1, file_) == 1;
    ok = ok && std::fwrite(frame.depth.data(), sizeof(uint16_t), sample_count,
                           file_) == sample_count;
    if (!ok) log_warn("failed to write frame record");
    return ok;
}

void Recorder::close() {
    if (!file_) return;
    std::fclose(file_);
    file_ = nullptr;
    log_info("recorder closed");
}

}  // namespace kc
