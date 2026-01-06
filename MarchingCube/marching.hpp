// marching.hpp
// This header file defines the functions and data structures used for the Marching Cubes algorithm.
// It includes functionality for generating a 3D mesh from a scalar field, computing normals, and exporting the mesh to a PLY file.

#ifndef MARCHING_CUBES_HPP
#define MARCHING_CUBES_HPP

#include <vector>
#include <functional>
#include <glm/glm.hpp>
#include <string>

// Generates a 3D mesh using the Marching Cubes algorithm.
// Parameters:
// - f: A scalar field function that takes (x, y, z) as input and returns a scalar value.
// - isovalue: The isosurface value to extract from the scalar field.
// - min: The minimum bounds of the scalar field.
// - max: The maximum bounds of the scalar field.
// - stepsize: The step size for sampling the scalar field.
// Returns: A vector of vertices representing the generated mesh.
std::vector<float> marching_cubes(
    std::function<float(float, float, float)> f,
    float isovalue,
    float min,
    float max,
    float stepsize
);

// Interpolates a vertex position along an edge between two points based on scalar values.
// Parameters:
// - p1, p2: The two points defining the edge.
// - valp1, valp2: The scalar values at p1 and p2.
// - isovalue: The isosurface value.
// Returns: The interpolated vertex position.
glm::vec3 interpolateVertex(
    const glm::vec3& p1, const glm::vec3& p2,
    float valp1, float valp2,
    float isovalue
);

// Computes flat normals for each triangle in the mesh.
// Parameters:
// - vertices: A vector of vertices representing the mesh.
// Returns: A vector of normals corresponding to the vertices.
std::vector<float> compute_normals(const std::vector<float>& vertices);

// Writes the vertices and normals to an ASCII PLY file.
// Parameters:
// - vertices: A vector of vertices representing the mesh.
// - normals: A vector of normals corresponding to the vertices.
// - filename: The name of the output PLY file.
void write_ply(const std::vector<float>& vertices, const std::vector<float>& normals, const std::string& filename);

#endif // MARCHING_CUBES_HPP