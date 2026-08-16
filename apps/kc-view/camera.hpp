#pragma once

#include "gl.hpp"

struct Camera {
    float yaw = 0.0f;
    float pitch = 0.0f;
    float distance = 2.5f;
    bool dragging = false;
    double last_x = 0.0;
    double last_y = 0.0;
};

void bind_camera_controls(GLFWwindow* window, Camera* camera);
void set_projection(int width, int height);
void apply_camera(const Camera& camera);
