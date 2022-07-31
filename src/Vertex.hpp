/**
 * @file      Vertex.hpp
 * @brief     Vertex data for vertex buffers.
 * @date      Fri Dec  3 18:08:22 2021
 * @author    Siddharth Mishra
 */

#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <glm/glm.hpp>

#include "Common.hpp"
#include "VertexInputDescription.hpp"

struct Vertex{
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 normal;

   static VertexInputDescription getVertexDescription();
};

#endif//VERTEX_HPP
