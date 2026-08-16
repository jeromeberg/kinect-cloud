# kinect-cloud

A small C++17 library to capture depth frames from Kinect and produce ready-to-use point clouds.

![c++17](https://img.shields.io/badge/C%2B%2B-17-blue?logo=c%2B%2B)

## Features

- Live capture from Kinect v1 (Xbox 360) using libfreenect
- Point cloud output (2 color modes)
- Live point cloud visualization (3D)

## Instructions

### Dependencies

- [libfreenect](https://github.com/OpenKinect/libfreenect)
- CMake
- GLFW3 and OpenGL (only for `kc-view`)

### Build

```bash
cmake -B build
cmake --build build
```

Builds `build/libkc.a` and binaries in `bin/`.

## Apps

### kc-view

Visualize the live point cloud in a window.

```bash
bin/kc-view [-v]
```

**Controls**

| Input           | Action       |
|-----------------|--------------|
| Mouse scroll    | Zoom         |
| Left-click drag | Orbit camera |

## Library

### Headers

| Header          | Content                           |
|-----------------|-----------------------------------|
| `kc/frame.hpp`  | `Frame`: one depth frame          |
| `kc/source.hpp` | `Source`: abstract frame provider |
| `kc/device.hpp` | `Device`: live kinect             |
| `kc/cloud.hpp`  | `Point`, `Cloud`, `ColorMode`     |
| `kc/log.hpp`    | `Level`                           |

### API

| Call                       | Description       | Return                |
|----------------------------|-------------------|-----------------------|
| `Device::open()`           | Open kinect       | `false` on fail       |
| `Device::next(Frame&)`     | Get next frame    | `false` on stop/error |
| `Device::stop()`           | Stop streaming    |                       |
| `Device::close()`          | Release device    |                       |
| `build_cloud(frame, color)`| Build point cloud | `Cloud`               |
| `set_log_level(Level)`     | Set min. log lvl  |                       |

### Cloud

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

### Example

Live capture, with log level set and colors turned off:

```cpp
#include <kc/cloud.hpp>
#include <kc/device.hpp>
#include <kc/log.hpp>

int main() {
    // set log level (Debug, Info, Warn, Error)
    kc::set_log_level(kc::Level::Info);

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
