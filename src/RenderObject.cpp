#include "RenderObject.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

void RenderObject::updateModelMatrix(){
    glm::mat4 translationMat = glm::translate(glm::mat4{ 1.0 }, position);
    glm::mat4 rotationMat = glm::rotate(glm::mat4{ 1.0 }, glm::radians(rotationAngle), rotationAxis);
    glm::mat4 scaleMat = glm::scale(glm::mat4{ 1.0 }, scale);
    modelMatrix = translationMat * rotationMat * scaleMat;
}

RenderObject::RenderObject(){
    updateModelMatrix();
}

RenderObject::RenderObject(Mesh* mesh, Material* material)
    : mesh(mesh), material(material){
    updateModelMatrix();
}

RenderObject::RenderObject(Mesh* mesh, Material* material,
                           const glm::vec3& pos, const glm::vec3& s,
                           const glm::vec3& rotAxis, float rotAngle)
    : mesh(mesh), material(material), position(pos), scale(s), rotationAxis(rotAxis), rotationAngle(rotAngle){
    updateModelMatrix();
}
