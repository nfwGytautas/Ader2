#pragma once

// Name
#include <string>

// Reference
#include "CommonTypes/reference.h"

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Forward declaration
class AderScene;

/**
 * C++ representation of Camera.cs
 */
class Camera
{
public:
    /**
     * Create camera instance for the scene
     */ 
	Camera(AderScene* pScene);

    /**
     * Updates the view matrix if needed
     */
    void update();

    /**
     * Returns the position of the camera
     */
    const glm::vec3& getPosition() const;

    /**
     * Sets the position of the camera
     */
    void setPosition(const glm::vec3& value);

    /**
     * Returns the rotation of the camera
     */
    const glm::vec3& getRotation() const;

    /**
     * Sets the rotation of the camera
     */
    void setRotation(const glm::vec3& value);

    /**
     * Get the view matrix of the camera
     */
    const glm::mat4& getViewMatrix() const;
private:
    /// Scene of the camera
    AderScene* m_pScene;

    /// Position of the object
    glm::vec3 m_position;

    /// Rotation of the object
    glm::vec3 m_rotation;

    /// The calculated matrix of the camera
    glm::mat4 m_viewMatrix = glm::mat4(1);

    /// Flag for update
    bool m_needsUpdate = false;
};
