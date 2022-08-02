#include "RenderObject.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

void RenderObject::updateModelMatrix(){
    glm::mat4 translationMat = glm::translate(glm::mat4{ 1.0 }, position);
    glm::mat4 rotationMat = glm::rotate(glm::mat4{ 1.0 }, glm::radians(rotationAngle), rotationAxis);
    glm::mat4 scaleMat = glm::scale(glm::mat4{ 1.0 }, scale);
    modelMatrix *= translationMat * rotationMat * scaleMat;
}

RenderObject::RenderObject(){
    updateModelMatrix();
}

RenderObject::RenderObject(Mesh* mesh, Material* material)
    : mesh(mesh), material(material){
    updateModelMatrix();
}

void RenderObject::setPosition(const glm::vec3& pos){
    position = pos;
    translationMatrix = glm::translate(glm::mat4{ 1.0 }, pos);
    modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
}

void RenderObject::move(const glm::vec3& moveVec){
    position += moveVec;
    translationMatrix = glm::translate(glm::mat4{ 1.0 }, moveVec);
    modelMatrix *= translationMatrix;
}

void RenderObject::setRotation(const glm::vec3& axis, float angle){
    rotationAxis = axis;
    rotationAngle = angle;
    rotationMatrix = glm::rotate(glm::mat4{ 1.0 }, glm::radians(angle), axis);
    modelMatrix *= rotationMatrix;
}

void RenderObject::setScale(const glm::vec3& scale){
    scaleMatrix = glm::scale(glm::mat4{ 1.0 }, scale);
    modelMatrix *= scaleMatrix;
}
