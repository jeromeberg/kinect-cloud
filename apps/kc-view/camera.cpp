#include "camera.hpp"

#include <algorithm>
#include <cmath>

namespace {

constexpr float TARGET_DEPTH = 2.0f;
constexpr float FIELD_OF_VIEW = 60.0f;
constexpr float NEAR_PLANE = 0.05f;
constexpr float FAR_PLANE = 100.0f;
constexpr float MIN_DISTANCE = 0.2f;
constexpr float MAX_DISTANCE = 20.0f;
constexpr float ZOOM_STEP = 1.1f;
constexpr float ORBIT_SPEED = 0.3f;
constexpr float MAX_PITCH = 89.0f;
constexpr float PI = 3.14159265f;

Camera* camera_of(GLFWwindow* window) {
    return static_cast<Camera*>(glfwGetWindowUserPointer(window));
}

void scroll_callback(GLFWwindow* window, double, double yoffset) {
    Camera* camera = camera_of(window);
    camera->distance *= std::pow(ZOOM_STEP, -static_cast<float>(yoffset));
    camera->distance =
        std::min(std::max(camera->distance, MIN_DISTANCE), MAX_DISTANCE);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int) {
    if (button != GLFW_MOUSE_BUTTON_LEFT) return;

    Camera* camera = camera_of(window);
    camera->dragging = action == GLFW_PRESS;
    glfwGetCursorPos(window, &camera->last_x, &camera->last_y);
}

void cursor_position_callback(GLFWwindow* window, double x, double y) {
    Camera* camera = camera_of(window);
    if (!camera->dragging) {
        camera->last_x = x;
        camera->last_y = y;
        return;
    }

    camera->yaw += static_cast<float>(x - camera->last_x) * ORBIT_SPEED;
    camera->pitch += static_cast<float>(y - camera->last_y) * ORBIT_SPEED;
    camera->pitch = std::min(std::max(camera->pitch, -MAX_PITCH), MAX_PITCH);
    camera->last_x = x;
    camera->last_y = y;
}

}  // namespace

void bind_camera_controls(GLFWwindow* window, Camera* camera) {
    glfwSetWindowUserPointer(window, camera);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
}

void set_projection(int width, int height) {
    float aspect = height > 0 ? static_cast<float>(width) / height : 1.0f;
    float top = NEAR_PLANE * std::tan(FIELD_OF_VIEW * PI / 360.0f);
    float right = top * aspect;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-right, right, -top, top, NEAR_PLANE, FAR_PLANE);
}

void apply_camera(const Camera& camera) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -camera.distance);
    glRotatef(camera.pitch, 1.0f, 0.0f, 0.0f);
    glRotatef(camera.yaw, 0.0f, 1.0f, 0.0f);
    glTranslatef(0.0f, 0.0f, TARGET_DEPTH);
}
