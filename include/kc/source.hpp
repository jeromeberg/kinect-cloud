#pragma once

#include <kc/frame.hpp>

namespace kc {

class Source {
 public:
    virtual ~Source() = default;

    virtual bool next(Frame& frame) = 0;
};

}  // namespace kc
