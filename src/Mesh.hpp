/**
 * @file      Mesh.hpp
 * @brief     Header of
 * @date      Fri Dec  3 18:48:57 2021
 * @author    Siddharth Mishra
 */

#ifndef MESH_HPP
#define MESH_HPP

#include "Vertex.hpp"
#include "AllocatedBuffer.hpp"
#include "ReturnCode.hpp"

struct Mesh{
    // vertex buffer
    std::vector<Vertex> vertices;
    AllocatedBuffer vertexBuffer;

    // indices buffer
    bool hasIndexBuffer = false;
    std::vector<uint32_t> indices;
    AllocatedBuffer indexBuffer;

    /**
     * @brief Load an obj file to mesh.
     * @param[in] filename.
     * @return SUCCESS on success, FAILED otherwise.
     */
    ReturnCode loadFromObj(const char* filename);
};

/**
 * @brief Create a sphere mesh in the given mesh object provided.
 *
 * @param[out] mesh Mesh object to store vertex data into.
 * @param[in] slices Number of vertical lines on sphere.
 * @param[in] circles Number of horizontal lines on sphere.
 * @param[in] color Color of sphere.
 * */
void createSphereMesh(Mesh& mesh, uint32_t slices, uint32_t circles, glm::vec3 color = {0.1f, 0.1f, 0.1f});

/**
 * @brief Create a rectangle mesh of given width and height.
 *
 * @param[out] mesh Mesh object to store vertex data into.
 * @param[in] width Width of plane
 * @param[out] height Height of plane
 * */
void createRectangleMesh(Mesh& mesh, float width, float height, glm::vec3 color = {0.1f, 0.1f, 0.1f});

/**
 * @brief Create a surface for given x, y vectors and z function.
 *
 * @param[out] mesh Mesh object to store vertex data into.
 * */
void createSurface(Mesh& mesh, const std::vector<float>& x, const std::vector<float>& y, float (*z)(float x, float y));

#endif//MESH_HPP
