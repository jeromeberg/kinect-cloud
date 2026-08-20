#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <kc/device.hpp>
#include <kc/log.hpp>
#include <kc/recorder.hpp>

namespace {

constexpr uint64_t MICROSECONDS_PER_SECOND = 1000000;

struct Args {
    const char* path = nullptr;
    double seconds = 0.0;
    bool verbose = false;
};

bool parse_args(int argc, char** argv, Args& args) {
    if (argc < 3 || argc > 4) return false;
    args.path = argv[1];

    bool has_duration = false;
    for (int i = 2; i < argc; ++i) {
        if (std::strcmp(argv[i], "-v") == 0) {
            args.verbose = true;
            continue;
        }
        char* end = nullptr;
        args.seconds = std::strtod(argv[i], &end);
        if (end == argv[i] || *end != '\0') return false;
        has_duration = true;
    }
    return has_duration;
}

void record(kc::Device& device, kc::Recorder& recorder, double seconds) {
    uint64_t limit_us =
        static_cast<uint64_t>(seconds * MICROSECONDS_PER_SECOND);

    kc::Frame frame;
    while (device.next(frame)) {
        recorder.write(frame);
        if (frame.timestamp >= limit_us) {
            device.stop();
            break;
        }
    }
}

}  // namespace

int main(int argc, char** argv) {
    Args args;
    if (!parse_args(argc, argv, args)) {
        std::fprintf(stderr, "usage: kc-record <file> <seconds> [-v]\n");
        return 1;
    }
    if (args.verbose) kc::set_log_level(kc::Level::Debug);

    kc::Device device;
    kc::Recorder recorder;
    if (!device.open() || !recorder.open(args.path)) return 1;

    record(device, recorder, args.seconds);

    recorder.close();
    device.close();
    return 0;
}
