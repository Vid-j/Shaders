#include "shader_utils.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <GL/glew.h>
#include <cstdio>

// Utility: read file contents
// Reads the contents of a file into a string
static std::string readFile(const char* filePath) {
    std::ifstream stream(filePath, std::ios::in);
    if (!stream.is_open()) {
        std::cerr << "Could not open " << filePath << "\n";
        return "";
    }
    std::stringstream sstr;
    sstr << stream.rdbuf();
    return sstr.str();
}

// Utility: compile individual shader
// Compiles a shader from source code and checks for errors
static GLuint compileShader(const std::string& code, GLenum type) {
    GLuint id = glCreateShader(type); // Create a shader object
    const char* source = code.c_str();
    glShaderSource(id, 1, &source, nullptr); // Set the shader source code
    glCompileShader(id); // Compile the shader

    // Check for compilation errors
    GLint result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (!result) {
        GLint length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> msg(length);
        glGetShaderInfoLog(id, length, nullptr, msg.data());
        std::cerr << "Shader compile error:\n" << msg.data() << "\n";
    }

    return id;
}

// Link all shader stages into a program
// Loads and links vertex, tessellation, geometry, and fragment shaders into a single program
GLuint LoadShaders(const char* vertex_file_path,
                   const char* tess_control_path,
                   const char* tess_eval_path,
                   const char* geometry_path,
                   const char* fragment_file_path) {

    // Compile each shader stage
    GLuint vs  = compileShader(readFile(vertex_file_path), GL_VERTEX_SHADER);
    GLuint tcs = compileShader(readFile(tess_control_path), GL_TESS_CONTROL_SHADER);
    GLuint tes = compileShader(readFile(tess_eval_path), GL_TESS_EVALUATION_SHADER);
    GLuint gs  = compileShader(readFile(geometry_path), GL_GEOMETRY_SHADER);
    GLuint fs  = compileShader(readFile(fragment_file_path), GL_FRAGMENT_SHADER);

    // Create a program and attach shaders
    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, tcs);
    glAttachShader(program, tes);
    glAttachShader(program, gs);
    glAttachShader(program, fs);

    // Link the program
    glLinkProgram(program);

    // Check for linking errors
    GLint result;
    glGetProgramiv(program, GL_LINK_STATUS, &result);
    if (!result) {
        GLint length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> msg(length);
        glGetProgramInfoLog(program, length, nullptr, msg.data());
        std::cerr << "Program link error:\n" << msg.data() << "\n";
    }

    // Delete shaders after linking (no longer needed)
    glDeleteShader(vs);
    glDeleteShader(tcs);
    glDeleteShader(tes);
    glDeleteShader(gs);
    glDeleteShader(fs);

    return program;
}

// Load a .bmp texture into OpenGL
// Loads a BMP file and creates an OpenGL texture
GLuint loadTextureBMP(const char* filepath) {
    // Open the BMP file
    FILE* file = fopen(filepath, "rb");
    if (!file) {
        std::cerr << "Failed to open BMP: " << filepath << std::endl;
        return 0;
    }

    // Read the BMP header
    unsigned char header[54];
    fread(header, 1, 54, file);

    // Verify that the file is a BMP
    if (header[0] != 'B' || header[1] != 'M') {
        std::cerr << "Not a BMP file: " << filepath << std::endl;
        fclose(file);
        return 0;
    }

    // Extract image metadata from the header
    unsigned int dataPos    = *(int*)&(header[0x0A]); // Offset to image data
    unsigned int width      = *(int*)&(header[0x12]); // Image width
    unsigned int height     = *(int*)&(header[0x16]); // Image height
    unsigned int imageSize  = *(int*)&(header[0x22]); // Image size

    // Default values if not specified in the header
    if (imageSize == 0) imageSize = width * height * 3; // 3 bytes per pixel (RGB)
    if (dataPos == 0) dataPos = 54; // Default BMP header size

    // Read the image data
    unsigned char* data = new unsigned char[imageSize];
    fread(data, 1, imageSize, file);
    fclose(file);

    // Generate and bind a texture in OpenGL
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Upload the image data to the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                 GL_BGR, GL_UNSIGNED_BYTE, data);

    // Set texture filtering and generate mipmaps
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Free the image data and return the texture ID
    delete[] data;
    return textureID;
}
