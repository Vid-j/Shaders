// MarchingCubes.cpp
// This file implements the Marching Cubes algorithm for generating a 3D mesh from a scalar field.
// It also includes functionality for computing normals and exporting the mesh to a PLY file.

#include "marching.hpp"
#include "TriTable.hpp" // Lookup table for Marching Cubes edge configurations
#include <glm/glm.hpp>
#include <array>
#include <vector>
#include <functional>
#include <fstream> // For PLY output
#include <iostream> // For std::cout and std::cerr

// Interpolates a vertex position along an edge between two points based on the scalar values at those points.
// Parameters:
// - p1, p2: The two points defining the edge.
// - valp1, valp2: The scalar values at p1 and p2.
// - isovalue: The isosurface value.
// Returns: The interpolated vertex position.
glm::vec3 interpolateVertex(const glm::vec3& p1, const glm::vec3& p2, float valp1, float valp2, float isovalue) {
    float t = (isovalue - valp1) / (valp2 - valp1); // Interpolation factor
    return p1 + t * (p2 - p1); // Interpolated position
}

// Implements the Marching Cubes algorithm to generate a 3D mesh from a scalar field.
// Parameters:
// - f: Scalar field function that takes (x, y, z) and returns a scalar value.
// - isovalue: The isosurface value to extract.
// - min, max: The bounds of the scalar field.
// - stepsize: The step size for sampling the scalar field.
// Returns: A vector of vertices representing the generated mesh.
std::vector<float> marching_cubes(
    std::function<float(float, float, float)> f,
    float isovalue,
    float min,
    float max,
    float stepsize
) {
    std::vector<float> vertices;

    // Cube corner offsets
    glm::vec3 cubeVerts[8] = {
        {0, 0, 0}, {1, 0, 0}, {1, 0, 1}, {0, 0, 1},
        {0, 1, 0}, {1, 1, 0}, {1, 1, 1}, {0, 1, 1}
    };

    // Edge vertex pairs
    const int edgeConnections[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };

    // Iterate through the scalar field
    for (float x = min; x < max; x += stepsize) {
        for (float y = min; y < max; y += stepsize) {
            for (float z = min; z < max; z += stepsize) {
                glm::vec3 cubePos = glm::vec3(x, y, z); // Current cube position
                glm::vec3 pos[8]; // Positions of cube corners
                float val[8];     // Scalar values at cube corners

                // Evaluate scalar field at cube corners
                for (int i = 0; i < 8; ++i) {
                    pos[i] = cubePos + cubeVerts[i] * stepsize;
                    val[i] = f(pos[i].x, pos[i].y, pos[i].z);
                }

                // Determine cube index based on scalar values
                int cubeIndex = 0;
                for (int i = 0; i < 8; ++i)
                    if (val[i] < isovalue)
                        cubeIndex |= (1 << i);

                // Get edge configuration from lookup table
                const int* triEdges = marching_cubes_lut[cubeIndex];
                if (triEdges[0] == -1) continue; // Skip if no triangles

                // Compute edge vertices
                glm::vec3 edgeVertex[12];
                for (int i = 0; i < 12; ++i) {
                    int v0 = edgeConnections[i][0];
                    int v1 = edgeConnections[i][1];
                    edgeVertex[i] = interpolateVertex(pos[v0], pos[v1], val[v0], val[v1], isovalue);
                }

                // Build triangles
                for (int i = 0; triEdges[i] != -1; i += 3) {
                    for (int j = 0; j < 3; ++j) {
                        glm::vec3 v = edgeVertex[triEdges[i + j]];
                        vertices.push_back(v.x);
                        vertices.push_back(v.y);
                        vertices.push_back(v.z);
                    }
                }
            }
        }
    }

    return vertices;
}

// Computes flat normals for each triangle in the mesh.
// Parameters:
// - vertices: A vector of vertices representing the mesh.
// Returns: A vector of normals corresponding to the vertices.
std::vector<float> compute_normals(const std::vector<float>& vertices) {
    std::vector<float> normals;
    for (size_t i = 0; i < vertices.size(); i += 9) {
        glm::vec3 v0(vertices[i],     vertices[i+1], vertices[i+2]);
        glm::vec3 v1(vertices[i+3],   vertices[i+4], vertices[i+5]);
        glm::vec3 v2(vertices[i+6],   vertices[i+7], vertices[i+8]);

        // Compute the normal using the cross product
        glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

        // Add the same normal for all three vertices of the triangle
        for (int j = 0; j < 3; ++j) {
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);
        }
    }
    return normals;
}

// Writes the vertices and normals to an ASCII PLY file.
// Parameters:
// - vertices: A vector of vertices representing the mesh.
// - normals: A vector of normals corresponding to the vertices.
// - filename: The name of the output PLY file.
void write_ply(const std::vector<float>& vertices, const std::vector<float>& normals, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    // Write PLY header
    size_t numVertices = vertices.size() / 3;
    file << "ply\n";
    file << "format ascii 1.0\n";
    file << "element vertex " << numVertices << "\n";
    file << "property float x\n";
    file << "property float y\n";
    file << "property float z\n";
    file << "property float nx\n";
    file << "property float ny\n";
    file << "property float nz\n";
    file << "end_header\n";

    // Write vertex and normal data
    for (size_t i = 0; i < numVertices; ++i) {
        file << vertices[i * 3] << " " << vertices[i * 3 + 1] << " " << vertices[i * 3 + 2] << " ";
        file << normals[i * 3] << " " << normals[i * 3 + 1] << " " << normals[i * 3 + 2] << "\n";
    }

    file.close();
    std::cout << "Wrote " << filename << " with " << numVertices << " vertices.\n";
}