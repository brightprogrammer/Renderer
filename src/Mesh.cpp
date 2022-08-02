#include "Mesh.hpp"
#include "ReturnCode.hpp"
#include "tiny_obj_loader.h"
#include "Math.hpp"

#include <cmath>
#include <iostream>

ReturnCode Mesh::loadFromObj(const char *filename){
    // attrib will contain the vertex arrays of the file
    tinyobj::attrib_t attrib;
    // shapes contains the info for each separate object in the file
    std::vector<tinyobj::shape_t> shapes;
    // materials contains the information about the material of each shape, but we won't use it.
    std::vector<tinyobj::material_t> materials;

    // error and warning output from the load function
    std::string warn;
    std::string err;

    // load the OBJ file
    tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename, nullptr);
    // make sure to output the warnings to the console, in case there are issues with the file
    if (!warn.empty()) {
        std::cout << "WARN: " << warn << std::endl;
    }
    // if we have any error, print it to the console, and break the mesh loading.
    // This happens if the file can't be found or is malformed
    if (!err.empty()) {
        std::cerr << err << std::endl;
        return FAILED;
    }

     // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++) {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {

            // hardcode loading to triangles
            size_t fv = 3;

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                //vertex position
                tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
                tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
                tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
                //vertex normal
                tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
                tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
                tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];

                //copy it into our vertex
                Vertex new_vert;
                new_vert.position.x = vx;
                new_vert.position.y = vy;
                new_vert.position.z = vz;

                new_vert.normal.x = nx;
                new_vert.normal.y = ny;
                new_vert.normal.z = nz;

                new_vert.color = new_vert.normal;

                vertices.push_back(new_vert);
            }
            index_offset += fv;
        }
    }

    std::cout << vertices.size() << "vertices loaded" << std::endl;

    return SUCCESS;
}

// create mesh.mesh and store in given mesh object
void createSphereMesh(Mesh& mesh, uint32_t slices, uint32_t circles, glm::vec3 color){
    float verticalAngleStep = PI / circles;
    float horizontalAngleStep = 2*PI / slices;
    float radius = 1.0f;

    for(uint32_t s = 0; s < slices; s++){
        Vertex v1, v2, v3;

        // generate vertices for topmost circle
        v1.position = sphericalToCartesian(radius, 0, 0);
        v2.position = sphericalToCartesian(radius, s*horizontalAngleStep, verticalAngleStep);
        v3.position = sphericalToCartesian(radius, (s+1)*horizontalAngleStep, verticalAngleStep);

        // normal is just the normalized position in this case
        v1.normal = glm::normalize(v1.position);
        v2.normal = glm::normalize(v2.position);
        v3.normal = glm::normalize(v3.position);

        // set color
        v1.color = color;
        v2.color = color;
        v3.color = color;

        // store triangle
        mesh.vertices.push_back(v1);
        mesh.vertices.push_back(v2);
        mesh.vertices.push_back(v3);

        // generate vertices for bottom-most circle
        v1.position = sphericalToCartesian(radius, 0, PI);
        v2.position = sphericalToCartesian(radius, s*horizontalAngleStep, (circles-1)*verticalAngleStep);
        v3.position = sphericalToCartesian(radius, (s+1)*horizontalAngleStep, (circles-1)*verticalAngleStep);

        // normal is just the normalized position in this case
        v1.normal = glm::normalize(v1.position);
        v2.normal = glm::normalize(v2.position);
        v3.normal = glm::normalize(v3.position);

        // color already set

        // store triangle
        mesh.vertices.push_back(v3);
        mesh.vertices.push_back(v2);
        mesh.vertices.push_back(v1);
    }

    // generate vertices leaving topmost circle and bottom-most circle
    for(uint32_t s = 0; s < slices; s++){
        for(uint32_t c = 1; c < circles - 1; c++){
            Vertex v1, v2, v3, v4;

            // calculate position of these points
            v1.position = sphericalToCartesian(radius, s*horizontalAngleStep, c*verticalAngleStep);
            v2.position = sphericalToCartesian(radius, (s+1)*horizontalAngleStep, c*verticalAngleStep);
            v3.position = sphericalToCartesian(radius, (s+1)*horizontalAngleStep, (c+1)*verticalAngleStep);
            v4.position = sphericalToCartesian(radius, s*horizontalAngleStep, (c+1)*verticalAngleStep);

            // normal is just the normalized position in this case
            v1.normal = glm::normalize(v1.position);
            v2.normal = glm::normalize(v2.position);
            v3.normal = glm::normalize(v3.position);
            v4.normal = glm::normalize(v4.position);

            v1.color = color;
            v2.color = color;
            v3.color = color;
            v4.color = color;

            // add first triangle to mesh
            mesh.vertices.push_back(v1);
            mesh.vertices.push_back(v4);
            mesh.vertices.push_back(v2);

            // add second triangle to mesh
            mesh.vertices.push_back(v2);
            mesh.vertices.push_back(v4);
            mesh.vertices.push_back(v3);
        }
    }
}

void createRectangleMesh(Mesh& mesh, float width, float height, glm::vec3 color){
    Vertex v;
    v.normal = {0, 0, 1};
    v.color = color;

    // construct first triangle
    v.position = {width/2, height/2, 0};
    mesh.vertices.push_back(v);

    v.position = {-width/2, height/2, 0};
    mesh.vertices.push_back(v);

    v.position = {-width/2, -height/2, 0};
    mesh.vertices.push_back(v);

    // construct second triangle
    v.position = {width/2, height/2, 0};
    mesh.vertices.push_back(v);

    v.position = {-width/2, -height/2, 0};
    mesh.vertices.push_back(v);

    v.position = {width/2, -height/2, 0};
    mesh.vertices.push_back(v);
}
