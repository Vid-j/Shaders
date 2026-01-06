#include "camera.hpp"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

// Static variables for camera angles and sensitivity
static float theta = glm::radians(90.0f);  // Horizontal angle (azimuth)
static float phi   = glm::radians(45.0f);  // Vertical angle (elevation)
static float sensitivity = 0.005f;         // Mouse sensitivity for rotation

// Variables to track mouse movement
static double lastX = -1, lastY = -1;      // Last mouse position
static bool rotating = false;              // Whether the mouse is rotating the camera

// Callback for mouse movement
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (!rotating) return; // Ignore if not rotating

    // Initialize last mouse position if not set
    if (lastX < 0 || lastY < 0) {
        lastX = xpos;
        lastY = ypos;
        return;
    }

    // Calculate mouse movement delta
    float dx = xpos - lastX;
    float dy = ypos - lastY;
    lastX = xpos;
    lastY = ypos;

    // Update camera angles based on mouse movement
    theta -= dx * sensitivity; // Horizontal rotation
    phi   -= dy * sensitivity; // Vertical rotation

    // Clamp vertical angle to avoid flipping
    phi = std::clamp(phi, 0.1f, glm::radians(179.9f));
}

// Callback for mouse button events
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) { // Left mouse button
        if (action == GLFW_PRESS) {
            rotating = true; // Start rotating
            glfwGetCursorPos(window, &lastX, &lastY); // Record initial mouse position
        } else if (action == GLFW_RELEASE) {
            rotating = false; // Stop rotating
            lastX = lastY = -1; // Reset mouse position
        }
    }
}

// Function to control the camera in a globe-like manner
void cameraControlsGlobe(glm::mat4& View, float radius) {
    // Register mouse callbacks only once
    static bool firstCall = true;
    if (firstCall) {
        firstCall = false;
        GLFWwindow* win = glfwGetCurrentContext(); // Get the current GLFW window
        glfwSetCursorPosCallback(win, mouse_callback); // Set mouse movement callback
        glfwSetMouseButtonCallback(win, mouse_button_callback); // Set mouse button callback
    }

    // Keyboard controls for camera rotation
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_LEFT) == GLFW_PRESS)  theta -= 0.02f; // Rotate left
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_RIGHT) == GLFW_PRESS) theta += 0.02f; // Rotate right
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_UP) == GLFW_PRESS)    phi -= 0.02f;   // Rotate up
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_DOWN) == GLFW_PRESS)  phi += 0.02f;   // Rotate down

    // Calculate camera position in spherical coordinates
    float x = radius * sinf(phi) * cosf(theta); // X-coordinate
    float y = radius * cosf(phi);              // Y-coordinate
    float z = radius * sinf(phi) * sinf(theta); // Z-coordinate

    // Define the camera's position, target, and up vector
    glm::vec3 eye = glm::vec3(x, y, z);        // Camera position
    glm::vec3 center = glm::vec3(0, 0, 0);     // Look-at target (origin)
    glm::vec3 up = glm::vec3(0, 1, 0);         // Up vector

    // Update the view matrix
    View = glm::lookAt(eye, center, up);
}
