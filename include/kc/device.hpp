#pragma once

#include <atomic>
#include <cstdint>
#include <kc/source.hpp>

namespace kc {

class Device : public Source {
 public:
    Device() = default;
    ~Device() override;

    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;

    bool open();
    void stop();
    void close();

    bool next(Frame& frame) override;

 private:
    uint64_t rebase_timestamp(uint32_t ticks);

    std::atomic<bool> streaming_{false};
    bool has_base_ = false;
    uint32_t last_ticks_ = 0;
    uint64_t elapsed_ticks_ = 0;
};

}  // namespace kc
