#pragma once

#include <GLFW/glfw3.h>     //Required for GLFWwindow type
#include <glm/glm.hpp>

void cameraControlsGlobe(glm::mat4& V, float radius);

// If you declared mouse_callback and mouse_button_callback here, make sure this comes after the GLFW include:
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
