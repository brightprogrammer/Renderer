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
    std::vector<Vertex> vertices;
    AllocatedBuffer vertexBuffer;

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
 * @param[in] radius Radius of sphere.
 * */
void createSphereMesh(Mesh& mesh, uint32_t slices, uint32_t circles, float radius, glm::vec3 color = {0.1f, 0.1f, 0.1f});

/**
 * @brief Create a rectangle mesh of given width and height.
 *
 * @param[out] mesh Mesh object to store vertex data into.
 * @param[in] width Width of plane
 * @param[out] height Height of plane
 * */
void createRectangleMesh(Mesh& mesh, float width, float height, glm::vec3 color = {0.1f, 0.1f, 0.1f});

#endif//MESH_HPP
