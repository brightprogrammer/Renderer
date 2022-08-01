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
     * @brief Create RenderObject.
     *
     * @param mesh Pointer to mesh to apply shape.
     * @param material Pointer to material to apply to object.
     * @param pos Position of object.
     * @param scale Scaling vector of object in 3D.
     * @param rotationAxis Axis to do rotation about.
     * @param rotationAngle Angles to rotate about rotation axis (in degrees).
     * */
    RenderObject(Mesh* mesh, Material* material,
                 const glm::vec3& pos, const glm::vec3& scale,
                 const glm::vec3& rotationAxis, float rotationAngle);

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
    inline void setPosition(const glm::vec3& pos) { position = pos; }

    /**
     * @brief Get current position of object.
     *
     * @return glm::vec3
     * */
    inline const glm::vec3& getPosition() { return position; }

    /**
     * @brief Set rotation of this render object.
     *
     * @param axis Rotation axis.
     * @param angle Rotation angle in degrees.
     * */
    inline void setRotation(const glm::vec3& axis, float angle){
        rotationAxis = axis;
        rotationAngle = angle;
    }

    /**
     * @brief Get current rotation axis of object.
     *
     * @return glm::vec3
     * */
    inline const glm::vec3& getRotationAxis(){ return rotationAxis; }

    /**
     * @brief Get current rotation of angle.
     *
     * @return float Angle in degrees
     * */
    inline float getRotationAngle() { return rotationAngle; };

    /**
     * @brief Set x, y and z scale of object.
     *
     * @param scale
     * */
    inline void setScale(const glm::vec3& s) { scale = s; }

    /**
     * @brief Get current scaling object.
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
        updateModelMatrix();
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

    // decides the position and oreintation of object in space
    glm::mat4 modelMatrix;
};


#endif // RENDEROBJECT_H_
