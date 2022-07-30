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

#endif//MESH_HPP
