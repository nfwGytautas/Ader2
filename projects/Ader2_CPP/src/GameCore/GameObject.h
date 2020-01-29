#pragma once

// Name
#include <string>

// Reference
#include "CommonTypes/reference.h"

// Forward declaration
struct Visual;
class AderScene;

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
private:
    /// Current visual of the game object
    Visual* m_pVisual = nullptr;

    /// Scene of the game object
    AderScene* m_pScene = nullptr;
};
