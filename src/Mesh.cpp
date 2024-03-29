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
    slices = slices * 2;
    float verticalAngleStep = PI / (circles+1);
    float horizontalAngleStep = 2*PI / slices;
    float radius = 1.0f;

    // topmost point of sphere
    Vertex v;
    // for topmost point both theta and phi is 0
    v.position = sphericalToCartesian(radius, 0, 0);
    v.normal = glm::normalize(v.position);
    v.color = color;
    mesh.vertices.push_back(v);

    // point just below topmost point but on surface of sphere
    // horizontally just below sphere so theta = 0
    // vertically it's just one step below
    v.position = sphericalToCartesian(radius, 0, verticalAngleStep);
    v.normal = glm::normalize(v.position);
    mesh.vertices.push_back(v);

    // index of first vertex in circle
    uint32_t baseIndex = 1;

    // generate vertices and indices for topmost circle
    // all vertices lie on same circle
    for(uint32_t s = 0; s < slices-1; s++){
        // just horizontal position on sphere changes
        v.position = sphericalToCartesian(radius, (s+1)*horizontalAngleStep, verticalAngleStep);
        // normal is just the normalized position in this case
        v.normal = glm::normalize(v.position);

        // store new vertex
        mesh.vertices.push_back(v);

        // anticlockwise is front face
        mesh.indices.push_back(0); // (baseIndex-1) topmost point is common to all
        mesh.indices.push_back(baseIndex + s); // s-th point on new circle
        mesh.indices.push_back(baseIndex + s+1); // (s+1)th point on new circle
    }

    // final triangle
    mesh.indices.push_back(0); // (baseIndex - 1) topmost point
    mesh.indices.push_back(slices); // (baseIndex + slices-1) actually (slices-1) point but on circle
    mesh.indices.push_back(1); // (baseIndex) actually first point (index = 0 on circle) but index is 1 in vertices vector

    // generate vertices leaving topmost circle and bottom-most circle
    // going from 1 to circles because first circle is already drawn
    // but we have to draw last circle in this loop only
    for(uint32_t c = 1; c < circles; c++){
        // store base index of previous circle
        uint32_t prevBaseIndex = baseIndex;

        // first point on new circle
        v.position = sphericalToCartesian(radius, 0, (c+1)*verticalAngleStep);
        v.normal = glm::normalize(v.position);
        mesh.vertices.push_back(v);

        // base index for this circle
        baseIndex = mesh.vertices.size() - 1;

        for(uint32_t s = 0; s < slices - 1; s++){
            // calculate position of these points
            v.position = sphericalToCartesian(radius, (s+1)*horizontalAngleStep, (c+1)*verticalAngleStep);
            // normal is just the normalized position in this case
            v.normal = glm::normalize(v.position);

            // add new vertex to this circle
            mesh.vertices.push_back(v);

            // indices for first triangle
            mesh.indices.push_back(prevBaseIndex + s); // s-th point on above circle
            mesh.indices.push_back(baseIndex + s); // s-th point on this circle
            mesh.indices.push_back(baseIndex + s+1); // (s+1)-th point on this circle

            // indices for second triangle
            mesh.indices.push_back(prevBaseIndex + s+1); // (s+1)-th point on above circle
            mesh.indices.push_back(prevBaseIndex + s); // s-th point on above circle
            mesh.indices.push_back(baseIndex + s+1); // (s+1)-th point on this circle

            // this (s+1)-th point on current circle is not pushed yet
            // but since we're generating points slice by slice
            // it will eventually be pushed to this circle and
            // since we're using relative addressing,
            // the index will remain as we calculated
        }

        // generate indices for last quad
        // indices for first triangle
        mesh.indices.push_back(prevBaseIndex + slices-1); // last point on above circle
        mesh.indices.push_back(baseIndex + slices-1); // s-last point on this circle
        mesh.indices.push_back(baseIndex); // first point on this circle

        // indices for second triangle
        mesh.indices.push_back(prevBaseIndex); // first point on above circle
        mesh.indices.push_back(prevBaseIndex + slices-1); // lastPoint on above circle
        mesh.indices.push_back(baseIndex); // first point on this circle
    }

    // bottom most point of sphere
    v.position = sphericalToCartesian(radius, 0, PI); // vertically opposite point to topmost point
    v.normal = glm::normalize(v.position);
    mesh.vertices.push_back(v);

    // index of bottom most point
    uint32_t lastIndex = mesh.vertices.size() - 1;

    // vertices for bottom-most circle is already generated
    // we just need to set indices matching with bottom-most point
    for(uint32_t s = 0; s < slices - 1; s++){
        // anticlockwise is front face
        mesh.indices.push_back(baseIndex + s); // s-th point on circle just above bottom-most point
        mesh.indices.push_back(lastIndex); // bottom-most point
        mesh.indices.push_back(baseIndex + s+1); // (s+1)-th point on circle just above bottom-most point
    }

    // final triangle
    mesh.indices.push_back(baseIndex + slices-1); // actually (slices-1) point but on circle
    mesh.indices.push_back(lastIndex); // bottom-most point
    mesh.indices.push_back(baseIndex); // actually first point (index = 0 on circle) but index is 1 in vertices vector

    // we are using index buffers now!!
    mesh.hasIndexBuffer = true;
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

    v.position = {width/2, -height/2, 0};
    mesh.vertices.push_back(v);

    mesh.indices.push_back(0);
    mesh.indices.push_back(1);
    mesh.indices.push_back(2);

    mesh.indices.push_back(0);
    mesh.indices.push_back(2);
    mesh.indices.push_back(3);

    mesh.hasIndexBuffer = true;
}

void createSurface(Mesh& mesh, const std::vector<float>& x, const std::vector<float>& y, float (*z)(float x, float y)){
    assert((x.size() == y.size()) && "SIZE OF X AND Y VECTORS MUST BE SAME");

    Vertex v1, v2, v3, v4;
    v1.color = {0.25,0.25,0.25};
    v2.color = {0.25,0.25,0.25};
    v3.color = {0.25,0.25,0.25};
    v4.color = {0.25,0.25,0.25};

    for(uint32_t i = 0; i < x.size()-1; i++){
        for(uint32_t j = 0; j < y.size()-1; j++){
            // generate vertices of this cell's quad
            v1.position = {x[i], z(x[i], y[j]), y[j]};
            v2.position = {x[i+1], z(x[i+1], y[j]) , y[j]};
            v3.position = {x[i+1], z(x[i+1], y[j+1]), y[j+1]};
            v4.position = {x[i], z(x[i], y[j+1]), y[j+1]};

            // normals correspondig to first triangle
            v4.normal = glm::cross(v2.position - v1.position, v4.position - v1.position);
            v2.normal = v4.normal;
            v1.normal = v4.normal;

            // v4.color = v4.normal;
            // v2.color = v2.normal;
            // v1.color = v1.normal;

            // push first triangle
            mesh.vertices.push_back(v4);
            mesh.vertices.push_back(v2);
            mesh.vertices.push_back(v1);

            // normals correspondig to second triangle
            v4.normal = glm::cross(v4.position - v3.position, v2.position - v3.position);
            v3.normal = v4.normal;
            v2.normal = v4.normal;

            // v4.color = v4.normal;
            // v3.color = v3.normal;
            // v2.color = v2.normal;

            // push second triangle
            mesh.vertices.push_back(v4);
            mesh.vertices.push_back(v3);
            mesh.vertices.push_back(v2);
        }
    }
}
