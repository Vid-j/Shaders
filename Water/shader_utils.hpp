#pragma once
#include <GL/glew.h>

// Function to load and compile shaders, and link them into a program
// Parameters:
// - vertex_file_path: Path to the vertex shader file
// - tess_control_path: Path to the tessellation control shader file
// - tess_eval_path: Path to the tessellation evaluation shader file
// - geometry_path: Path to the geometry shader file
// - fragment_file_path: Path to the fragment shader file
// Returns:
// - GLuint: The ID of the linked shader program
GLuint LoadShaders(const char* vertex_file_path,
                   const char* tess_control_path,
                   const char* tess_eval_path,
                   const char* geometry_path,
                   const char* fragment_file_path);

// Function to load a BMP texture into OpenGL
// Parameters:
// - filepath: Path to the BMP file
// Returns:
// - GLuint: The ID of the generated OpenGL texture
GLuint loadTextureBMP(const char* filepath);
