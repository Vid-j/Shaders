#include "Camera.hpp"
#include <glm/gtc/constants.hpp>

// Constructor: Initializes the camera with given spherical coordinates (r, theta, phi)
// and sets default sensitivity values for mouse and scroll input.
Camera::Camera(float initR, float initTheta, float initPhi)
    : r(initR), theta(initTheta), phi(initPhi),
      mouseSensitivity(0.1f), scrollSensitivity(0.1f) {
}

// Calculates the camera's position in Cartesian coordinates based on spherical coordinates (r, theta, phi).
glm::vec3 Camera::getPosition() const {
    float x = r * sin(phi) * cos(theta); // X-coordinate
    float y = r * cos(phi);              // Y-coordinate
    float z = r * sin(phi) * sin(theta); // Z-coordinate
    return glm::vec3(x, y, z);
}

// Generates the view matrix for the camera using the position, target, and up vector.
// The camera looks at the origin (0, 0, 0) with the Y-axis as the up direction.
glm::mat4 Camera::getViewMatrix() const {
    glm::vec3 position = getPosition(); // Get the camera's position
    return glm::lookAt(position, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

// Processes mouse movement input to adjust the camera's spherical coordinates (theta and phi).
// xoffset: Horizontal mouse movement
// yoffset: Vertical mouse movement
// constrainPitch: Whether to constrain the pitch (phi) to avoid flipping the camera.
void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    theta += glm::radians(xoffset * mouseSensitivity); // Adjust theta (horizontal rotation)
    phi   += glm::radians(yoffset * mouseSensitivity); // Adjust phi (vertical rotation)

    // Constrain the pitch (phi) to avoid flipping the camera upside down
    if (constrainPitch) {
        const float minPhi = 0.1f;                     // Minimum pitch angle
        const float maxPhi = glm::pi<float>() - 0.1f; // Maximum pitch angle
        if (phi < minPhi) phi = minPhi;
        if (phi > maxPhi) phi = maxPhi;
    }
}

// Processes mouse scroll input to adjust the camera's radius (r).
// yoffset: Scroll input (positive for zooming in, negative for zooming out).
void Camera::processMouseScroll(float yoffset) {
    r -= yoffset * scrollSensitivity; // Adjust the radius based on scroll input
    if (r < 1.0f) r = 1.0f;           // Constrain the radius to a minimum value of 1.0
}