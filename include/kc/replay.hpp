#pragma once

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <kc/source.hpp>
#include <string>

namespace kc {

class Replay : public Source {
 public:
    Replay() = default;
    ~Replay() override;

    Replay(const Replay&) = delete;
    Replay& operator=(const Replay&) = delete;

    bool open(const std::string& path, bool realtime = true);

    void close();

    bool next(Frame& frame) override;

 private:
    bool read_header();
    bool read_record(Frame& frame);
    void pace(uint64_t timestamp);

    FILE* file_ = nullptr;
    bool realtime_ = true;
    uint32_t width_ = 0;
    uint32_t height_ = 0;
    bool has_start_ = false;
    std::chrono::steady_clock::time_point start_time_;
};

}  // namespace kc
