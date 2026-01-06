#pragma once

#include "camera.hpp"
#include "shader_utils.hpp"

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// Declare if not already
GLuint loadTextureBMP(const char* filepath); // Function to load BMP textures

// Class representing a plane mesh with tessellation and texture support
class PlaneMesh {
    // Vertex data
    std::vector<float> verts;   // Vertex positions
    std::vector<float> normals; // Vertex normals
    std::vector<int> indices;   // Indices for drawing the mesh

    // Mesh properties
    float min, max;             // Range of the plane (min and max coordinates)
    glm::vec4 modelColor;       // Color of the model
    int numVerts, numIndices;   // Number of vertices and indices

    // OpenGL objects
    GLuint vao, vboVerts, vboNormals, ebo; // Vertex Array Object, Vertex/Normal Buffers, Element Buffer
    GLuint shaderProgram;                  // Shader program ID
    GLuint waterTex, dispTex;              // Texture IDs for water and displacement maps

    // Function to generate the plane mesh as quads
    void planeMeshQuads(float min, float max, float stepsize) {
        float y = 0; // Fixed height for the plane
        // Generate vertices and normals
        for (float x = min; x <= max; x += stepsize) {
            for (float z = min; z <= max; z += stepsize) {
                verts.push_back(x); verts.push_back(y); verts.push_back(z); // Vertex position
                normals.push_back(0); normals.push_back(1); normals.push_back(0); // Normal pointing up
            }
        }

        // Generate indices for quads
        int nCols = (max - min) / stepsize + 1; // Number of columns/rows in the grid
        for (int i = 0; i < nCols - 1; ++i) {
            for (int j = 0; j < nCols - 1; ++j) {
                indices.push_back(i * nCols + j);
                indices.push_back(i * nCols + j + 1);
                indices.push_back((i + 1) * nCols + j + 1);
                indices.push_back((i + 1) * nCols + j);
            }
        }
    }

public:
    // Constructor to initialize the plane mesh
    PlaneMesh(float min, float max, float stepsize) {
        this->min = min;
        this->max = max;
        modelColor = glm::vec4(0, 1.0f, 1.0f, 1.0f); // Default color (cyan)

        // Generate the plane mesh
        planeMeshQuads(min, max, stepsize);
        numVerts = verts.size() / 3; // Each vertex has 3 components (x, y, z)
        numIndices = indices.size();

        // Load shaders and textures
        shaderProgram = LoadShaders("WaterShader.vertexshader",
                                    "WaterShader.tcs",
                                    "WaterShader.tes",
                                    "WaterShader.geoshader",
                                    "WaterShader.fragmentshader");

        waterTex = loadTextureBMP("Assets/water.bmp"); // Load water texture
        dispTex = loadTextureBMP("Assets/displacement-map1.bmp"); // Load displacement map

        // Check if textures loaded successfully
        if (waterTex == 0 || dispTex == 0)
            std::cerr << "⚠️ Warning: One or more textures failed to load.\n";

        // Set up OpenGL buffers
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // Vertex buffer
        glGenBuffers(1, &vboVerts);
        glBindBuffer(GL_ARRAY_BUFFER, vboVerts);
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // Position attribute
        glEnableVertexAttribArray(0);

        // Normal buffer
        glGenBuffers(1, &vboNormals);
        glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // Normal attribute
        glEnableVertexAttribArray(1);

        // Element buffer
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

        glBindVertexArray(0); // Unbind VAO
    }

    // Function to draw the plane mesh
    void draw(glm::vec3 lightPos, glm::mat4 V, glm::mat4 P) {
        glUseProgram(shaderProgram); // Use the shader program

        // Model, View, Projection matrices
        glm::mat4 Model = glm::mat4(1.0f); // Identity matrix for the model
        glm::mat4 MVP = P * V * Model;    // Combined MVP matrix
        glm::vec3 eye = glm::vec3(glm::inverse(V)[3]); // Camera position (extracted from View matrix)

        // Pass matrices to the shader
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "M"), 1, GL_FALSE, glm::value_ptr(Model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "V"), 1, GL_FALSE, glm::value_ptr(V));

        // Pass lighting and time information to the shader
        glUniform3fv(glGetUniformLocation(shaderProgram, "LightPosition_worldspace"), 1, &lightPos[0]);
        glUniform3fv(glGetUniformLocation(shaderProgram, "EyePosition_worldspace"), 1, &eye[0]);
        glUniform1f(glGetUniformLocation(shaderProgram, "time"), glfwGetTime());

        // Pass tessellation and texture parameters to the shader
        glUniform1f(glGetUniformLocation(shaderProgram, "outerTess"), 16.0f); // Outer tessellation level
        glUniform1f(glGetUniformLocation(shaderProgram, "innerTess"), 16.0f); // Inner tessellation level
        glUniform1f(glGetUniformLocation(shaderProgram, "texScale"), 10.0f);  // Texture scaling factor
        glUniform2f(glGetUniformLocation(shaderProgram, "texOffset"), 0.0f, 0.0f); // Texture offset
        glUniform4fv(glGetUniformLocation(shaderProgram, "modelcolor"), 1, glm::value_ptr(modelColor)); // Model color

        // Bind and pass textures to the shader
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, waterTex);
        glUniform1i(glGetUniformLocation(shaderProgram, "waterTexture"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, dispTex);
        glUniform1i(glGetUniformLocation(shaderProgram, "displacementTexture"), 1);

        // Draw the mesh using tessellation patches
        glBindVertexArray(vao);
        glPatchParameteri(GL_PATCH_VERTICES, 4); // Specify 4 vertices per patch
        glDrawElements(GL_PATCHES, numIndices, GL_UNSIGNED_INT, 0); // Draw the mesh
        glBindVertexArray(0); // Unbind VAO
    }
};
