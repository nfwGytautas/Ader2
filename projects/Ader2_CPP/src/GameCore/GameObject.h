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
struct Visual;
class AderScene;


/**
 * Struct used to describe the transformation of a single game object
 */
struct Transform
{
    /// Position of the object in the scene
    glm::vec3 Position = glm::vec3(0);

    /// Rotation of the object in the scene
    glm::vec3 Rotation = glm::vec3(0);

    /// Scale of the object in the scene
    glm::vec3 Scale = glm::vec3(1);
};


/**
 * C++ representation of GameObject.cs
 */
class GameObject
{
public:
    GameObject(AderScene* scene);

    /**
     * Returns the visual of the game object
     */
    Visual* getVisual();

    /**
     * Returns the game object scene
     */
    AderScene* getScene();

    /**
     * Sets game object visual to the one specified
     */
    void setVisual(Visual* visual);

    /**
     * Returns the transformation matrix of the object
     */
    glm::mat4 getTransformation();

    /**
     * Returns true if the transformation matrix changed since the
     * last call to getTransformation
     */
    bool transformChanged();

    /**
     * Get transform data, does not flag the game object as needing
     * an update
     */
    const Transform& getTransform() const;

    /**
     * Get transform data reference, does flag the game object as needing
     * an update even if there were no changes to it
     */
    Transform& setTransform();
private:
    /// Current visual of the game object
    Visual* m_pVisual = nullptr;

    /// Scene of the game object
    AderScene* m_pScene = nullptr;

    /// Transformation data for this game object
    Transform m_transform;

    /**
     * If true then the next time a game object is updated
     * a new transformation will be created
     */
    bool m_transformUpdate = false;
};
