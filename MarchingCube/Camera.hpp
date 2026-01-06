#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// The Camera class represents a camera in 3D space using spherical coordinates.
// It provides functionality to calculate the camera's position, generate a view matrix,
// and handle user input for camera movement and zoom.
class Camera {
public:
    // Spherical coordinate parameters
    float r;       // Distance from the origin (radius)
    float theta;   // Horizontal angle (azimuth) in radians
    float phi;     // Vertical angle (elevation) in radians

    // Sensitivity settings
    float mouseSensitivity;  // Sensitivity for mouse movement
    float scrollSensitivity; // Sensitivity for scroll input

    // Constructor
    // Initializes the camera with given spherical coordinates (r, theta, phi).
    // Default values:
    // - r = 5.0f (distance from the origin)
    // - theta = 45 degrees in radians (horizontal angle)
    // - phi = 45 degrees in radians (vertical angle)
    Camera(float initR = 5.0f, float initTheta = glm::radians(45.0f), float initPhi = glm::radians(45.0f));

    // Get the camera position in Cartesian coordinates.
    // Converts spherical coordinates (r, theta, phi) to Cartesian coordinates (x, y, z).
    glm::vec3 getPosition() const;

    // Get the view matrix for the camera.
    // The view matrix is calculated using the camera's position, looking at the origin (0, 0, 0),
    // with the Y-axis as the up direction.
    glm::mat4 getViewMatrix() const;

    // Update the camera's spherical coordinates (theta and phi) based on mouse movement.
    // Parameters:
    // - xoffset: Horizontal mouse movement
    // - yoffset: Vertical mouse movement
    // - constrainPitch: Whether to constrain the pitch (phi) to avoid flipping the camera
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

    // Update the camera's radius (r) based on scroll input.
    // Parameters:
    // - yoffset: Scroll input (positive for zooming in, negative for zooming out)
    void processMouseScroll(float yoffset);
};

#endif // CAMERA_HPP