# kinect-cloud

C++17 library for Kinect v1 depth capture and point cloud generation.

![c++17](https://img.shields.io/badge/C%2B%2B-17-blue?logo=c%2B%2B)

## Features

- Live capture from Kinect v1 (Xbox 360) using libfreenect
- Point cloud output (2 color modes)
- Live 3D point cloud visualization
- Record and replay depth frames to/from file

## Instructions

### Dependencies

- [libfreenect](https://github.com/OpenKinect/libfreenect)
- CMake
- GLFW3 and OpenGL (`kc-view` only)

### Build

```bash
cmake -B build
cmake --build build
```

Builds `build/libkc.a` and binaries in `bin/`.

## Apps

### kc-view

Visualize a point cloud in a window from device or from file.

```bash
bin/kc-view [file] [-v]
```

**Controls**

| Input           | Action       |
|-----------------|--------------|
| Mouse scroll    | Zoom         |
| Left-click drag | Orbit camera |

### kc-record

Record live frames to a `.kc` file.

```bash
bin/kc-record <file> <seconds> [-v]
```

## Library

### Device

Header: `kc/device.hpp`

| Call                   | Description    | Return                |
|------------------------|----------------|-----------------------|
| `Device::open()`       | Open kinect    | `false` on fail       |
| `Device::next(Frame&)` | Get next frame | `false` on stop/error |
| `Device::stop()`       | Stop streaming |                       |
| `Device::close()`      | Release device |                       |

**Example**

Live capture, with log level set and colors turned off:

```cpp
#include <kc/cloud.hpp>
#include <kc/device.hpp>

int main() {
    // open device
    kc::Device device;
    if (!device.open())
        return 1;

    kc::Frame frame;
    // get frames from device
    while (device.next(frame)) {
        // build cloud, set ColorMode (DepthGradient, None)
        kc::Cloud cloud = kc::build_cloud(frame, kc::ColorMode::None);
        // ...
    }

    // close device
    device.close();
    return 0;
}
```

### Replay

Header: `kc/replay.hpp`

| Call                           | Description    | Return               |
|--------------------------------|----------------|----------------------|
| `Replay::open(path, realtime)` | Open file      | `false` on fail      |
| `Replay::next(Frame&)`         | Get next frame | `false` on EOF/error |
| `Replay::close()`              | Close file     |                      |

**Example**

Replay a file:

```cpp
#include <kc/cloud.hpp>
#include <kc/replay.hpp>

int main() {
    kc::Replay replay;
    if (!replay.open("capture.kc"))
        return 1;

    kc::Frame frame;
    while (replay.next(frame)) {
        kc::Cloud cloud = kc::build_cloud(frame, kc::ColorMode::None);
        // ...
    }
    return 0;
}
```

### Cloud

Header: `kc/cloud.hpp`

| Call                        | Description       | Return  |
|-----------------------------|-------------------|---------|
| `build_cloud(frame, color)` | Build point cloud | `Cloud` |

```cpp
// Point
struct Point {
    float x, y, z;
    uint8_t r, g, b;
};

// Cloud
struct Cloud {
    std::vector<Point> points;
    uint64_t timestamp;
};
```

### Recorder

Header: `kc/recorder.hpp`

| Call                      | Description         | Return          |
|---------------------------|---------------------|-----------------|
| `Recorder::open(path)`    | Create file         | `false` on fail |
| `Recorder::write(Frame&)` | Write frame to file | `false` on fail |
| `Recorder::close()`       | Close file          |                 |

**Example**

Record to a file:

```cpp
#include <kc/device.hpp>
#include <kc/recorder.hpp>

int main() {
    kc::Device device;
    kc::Recorder recorder;
    if (!device.open() || !recorder.open("capture.kc"))
        return 1;

    kc::Frame frame;
    while (device.next(frame))
        recorder.write(frame);
    return 0;
}
```

### Log

Header: `kc/log.hpp`

| Call                   | Description            |
|------------------------|------------------------|
| `set_log_level(Level)` | Set minimum log level  |

```cpp
//#include "kc/log.hpp"

kc::set_log_level(kc::Level::Debug);
kc::set_log_level(kc::Level::Info);
kc::set_log_level(kc::Level::Warn); // default
kc::set_log_level(kc::Level::Error);
```




