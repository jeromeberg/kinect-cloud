#pragma once

#include <cstdint>
#include <cstdio>
#include <kc/frame.hpp>
#include <string>

namespace kc {

class Recorder {
 public:
    Recorder() = default;
    ~Recorder();

    Recorder(const Recorder&) = delete;
    Recorder& operator=(const Recorder&) = delete;

    bool open(const std::string& path);

    bool write(const Frame& frame);

    void close();

 private:
    bool write_header();

    FILE* file_ = nullptr;
    bool has_base_ = false;
    uint64_t base_timestamp_ = 0;
};

}  // namespace kc
