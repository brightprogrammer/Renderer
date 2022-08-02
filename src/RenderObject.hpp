#ifndef RENDEROBJECT_H_
#define RENDEROBJECT_H_

#include "Mesh.hpp"
#include "Material.hpp"

struct RenderObject {
    /**
     * @brief Create object with no material.
     * Default position and scaling.
     * */
    RenderObject();

    /**
     * @brief Create RenderObject with given mesh and material.
     * This will assume default position and scaling of object.
     *
     * @param mesh Pointer to mesh to apply shape.
     * @param material Pointer to material to apply to object.
     * */
    RenderObject(Mesh* mesh, Material* material);

    /**
     * @brief Set this mesh for this object.
     *
     * @param mesh Pointer to mesh to apply to this object.
     * */
    inline void setMesh(Mesh* m) { mesh = m; }

    /**
     * @brief Get pointer to applied mesh object.
     *
     * @return Mesh*
     * */
    inline Mesh* getMesh() { return mesh; }

    /**
     * @brief Apply given material to this object.
     *
     * @param material Pointer to material to apply to this object.
     * */
    inline void setMaterial(Material* mat) { material = mat; }

    /**
     * @brief Get applied material to this object.
     *
     * @return Material*
     * */
    inline Material* getMaterial() { return material; }

    /**
     * @brief Set position of this render object.
     *
     * @param pos
     * */
    void setPosition(const glm::vec3& pos);

    /**
     * @brief Get current position of object.
     *
     * @return glm::vec3
     * */
    inline const glm::vec3& getPosition() { return position; }

    /**
     * @brief Move object by given vector.
     *
     * @param moveVec providing direction and magnitude of motion.
     * */
    void move(const glm::vec3& moveVec);

    /**
     * @brief Set rotation of this render object.
     * This will be added over previous rotations of object.
     * Last rotation data won't be stored.
     *
     * @param axis Rotation axis.
     * @param angle Rotation angle in degrees.
     * */
    void setRotation(const glm::vec3& axis, float angle);

    /**
     * @brief Get last rotation axis of object.
     *
     * @return glm::vec3
     * */
    inline const glm::vec3& getRotationAxis(){ return rotationAxis; }

    /**
     * @brief Get last rotation angle of object.
     *
     * @return float Angle in degrees
     * */
    inline float getRotationAngle() { return rotationAngle; };

    /**
     * @brief Set x, y and z scale of object.
     *
     * @param scale
     * */
    void setScale(const glm::vec3& s);

    /**
     * @brief Get last scaling vector object.
     *
     * @return glm::vec3
     * */
    inline const glm::vec3& getScale() { return scale; }

    /**
     * @brief Get model matrix of this object.
     *
     * @return glm::mat4
     * */
    inline const glm::mat4& getModelMatrix() {
        return modelMatrix;
    };
private:
    void updateModelMatrix();

    Mesh* mesh = nullptr;
    Material* material = nullptr;

    glm::vec3 position = {0, 0, 0};
    glm::vec3 scale = {1, 1, 1};
    glm::vec3 rotationAxis = {1, 0, 0};
    float rotationAngle = 0;

    //decides the position and oreintation of object in space
    glm::mat4 translationMatrix = glm::mat4{1.f};
    glm::mat4 rotationMatrix = glm::mat4{1.f};
    glm::mat4 scaleMatrix = glm::mat4{1.f};
    glm::mat4 modelMatrix = glm::mat4{1.f};
};


#endif // RENDEROBJECT_H_
