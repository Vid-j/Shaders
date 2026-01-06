// Main application for rendering a 3D scene using OpenGL.
// Includes camera controls, shader loading, and rendering of a mesh generated using marching cubes.

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.hpp"
#include "marching.hpp"
#include <fstream>     // For file reading
#include <sstream>     // For string manipulation
#include <string>      // For std::string
#include <iostream>    // For console output

// Global variables for camera and mouse interaction
Camera camera;               // Camera object
bool mousePressed = false;   // Tracks if the left mouse button is pressed
double lastX = 0.0, lastY = 0.0; // Last mouse position
bool firstMouse = true;      // Tracks if this is the first mouse movement
GLuint VAO, VBO[2];          // Vertex Array Object and Vertex Buffer Objects

// Callback for mouse button events
// Tracks when the left mouse button is pressed or released
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        mousePressed = (action == GLFW_PRESS);
        if (mousePressed)
            firstMouse = true; // Reset mouse tracking on new press
    }
}

// Callback for mouse movement events
// Updates the camera's orientation based on mouse movement
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if (!mousePressed) return; // Only process if the left mouse button is pressed

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX; // Horizontal movement
    float yoffset = ypos - lastY; // Vertical movement
    lastX = xpos;
    lastY = ypos;

    camera.processMouseMovement(xoffset, yoffset); // Update camera orientation
}

// Callback for scroll events
// Updates the camera's zoom level based on scroll input
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.processMouseScroll((float)yoffset);
}

// Function to load and compile shaders from files
GLuint loadShaders(const char* vertexPath, const char* fragmentPath) {
    // Helper lambda to read a file into a string
    auto readFile = [](const char* path) -> std::string {
        std::ifstream file(path);
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    };

    // Read shader source code
    std::string vertexCode = readFile(vertexPath);
    std::string fragmentCode = readFile(fragmentPath);
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // Compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);
    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "Vertex shader compilation failed:\n" << infoLog << std::endl;
    }

    // Compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "Fragment shader compilation failed:\n" << infoLog << std::endl;
    }

    // Link shaders into a program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "Shader program linking failed:\n" << infoLog << std::endl;
    }

    // Clean up shaders (no longer needed after linking)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Set GLFW window hints for OpenGL version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a GLFW window
    GLFWwindow* window = glfwCreateWindow(800, 600, "CS3388 Camera Test", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Load shaders
    GLuint shaderProgram = loadShaders("vertex_shader.glsl", "fragment_shader.glsl");

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Set callback functions for mouse and scroll input
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Define scalar function for marching cubes
    auto scalarFunction = [](float x, float y, float z) {
        return cos(x * 2) - sin(y * 2) - sin(z * 2);
    };

    float isovalue = -1.5f; // Isovalue for the scalar field
    float min = -5.0f;      // Minimum bounds for the field
    float max = 5.0f;       // Maximum bounds for the field
    float step = 0.2f;      // Step size for sampling

    // Generate mesh using marching cubes
    auto vertices = marching_cubes(scalarFunction, isovalue, min, max, step);
    auto normals = compute_normals(vertices);

    // Export mesh to a .ply file
    write_ply(vertices, normals, "output.ply");

    // Upload mesh data to GPU
    glGenVertexArrays(1, &VAO);
    glGenBuffers(2, VBO);

    glBindVertexArray(VAO);

    // Vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // layout(location = 0)
    glEnableVertexAttribArray(0);

    // Normal buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // layout(location = 1)
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); // Unbind VAO

    // Main rendering loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Calculate transformation matrices
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.f / 600.f, 0.1f, 100.f);
        glm::mat4 model = glm::mat4(1.0f);

        // Use shader program and set uniforms
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // Render the mesh
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);
        glBindVertexArray(0);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Terminate GLFW
    glfwTerminate();
    return 0;
}