#include <cstdio>
#include <cstring>
#include <kc/cloud.hpp>
#include <kc/device.hpp>
#include <kc/log.hpp>
#include <kc/replay.hpp>

#include "camera.hpp"
#include "gl.hpp"

namespace {

constexpr int WINDOW_WIDTH = 1024;
constexpr int WINDOW_HEIGHT = 768;
constexpr float POINT_SIZE = 2.0f;

struct Args {
    const char* path = nullptr;
    bool verbose = false;
};

bool parse_args(int argc, char** argv, Args& args) {
    if (argc > 3) return false;

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "-v") == 0) {
            args.verbose = true;
            continue;
        }
        if (args.path) return false;
        args.path = argv[i];
    }
    return true;
}

kc::Source* open_source(const Args& args, kc::Device& device,
                        kc::Replay& replay) {
    if (args.path) return replay.open(args.path) ? &replay : nullptr;
    return device.open() ? &device : nullptr;
}

GLFWwindow* create_window() {
    if (!glfwInit()) {
        kc::log_error("cannot initialize glfw");
        return nullptr;
    }

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
                                          "kc-view", nullptr, nullptr);
    if (!window) {
        kc::log_error("cannot create window");
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glEnable(GL_DEPTH_TEST);
    glPointSize(POINT_SIZE);
    glClearColor(0.06f, 0.06f, 0.09f, 1.0f);
    return window;
}

void draw_cloud(const kc::Cloud& cloud) {
    if (cloud.points.empty()) return;

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof(kc::Point), &cloud.points[0].x);
    glColorPointer(3, GL_UNSIGNED_BYTE, sizeof(kc::Point), &cloud.points[0].r);
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(cloud.points.size()));
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void render(GLFWwindow* window, const Camera& camera, const kc::Cloud& cloud) {
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(window, &width, &height);

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    set_projection(width, height);
    apply_camera(camera);
    glScalef(1.0f, -1.0f, -1.0f);
    draw_cloud(cloud);
    glfwSwapBuffers(window);
}

void run(GLFWwindow* window, kc::Source& source) {
    Camera camera;
    bind_camera_controls(window, &camera);

    kc::Frame frame;
    kc::Cloud cloud;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (!source.next(frame)) break;
        cloud = kc::build_cloud(frame);
        render(window, camera, cloud);
    }
}

}  // namespace

int main(int argc, char** argv) {
    Args args;
    if (!parse_args(argc, argv, args)) {
        std::fprintf(stderr, "usage: kc-view [file] [-v]\n");
        return 1;
    }
    if (args.verbose) kc::set_log_level(kc::Level::Debug);

    kc::Device device;
    kc::Replay replay;
    kc::Source* source = open_source(args, device, replay);
    if (!source) return 1;

    GLFWwindow* window = create_window();
    if (!window) return 1;

    run(window, *source);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
