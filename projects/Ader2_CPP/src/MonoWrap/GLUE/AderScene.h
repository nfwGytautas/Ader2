#pragma once

#include "MonoWrap/MonoManager.h"

// Objects array
#include "GameCore/GameObject.h"

// VAO, Shader
#include "OpenGLModules/GLContext.h"

// Asset base class
#include "CommonTypes/Asset.h"

/**
 * Visual struct is used to define a single way something looks.
 * When creating a GameObject a valid visual must first be created,
 * then it can be assigned to the GameObject.
 */
struct Visual : public Asset
{
    /// Objects using this visual
    std::vector<GameObject*> Objects;

    /// Reference to the vertex array of this visual
    VAO* VAO;

    /// Map of textures, key symbolizes the texture slot
    std::unordered_map<int, Texture*> Textures;

    /// Reference to the shader of this visual
    Shader* Shader;
};


/**
 * C++ representation of the AderScene.cs
 */
struct AderSceneBase
{
    /// Class of ader script
    Memory::reference<SharpClass> Klass;

    /// Constructor of AderScene
    Memory::reference<SharpMethod> Constructor;

    /// LoadAssets method of AderScene
    Memory::reference<SharpMethod> LoadAssets;

    /// CIntance field of AderScene
    Memory::reference<SharpField> _CInstance;
};


/**
 * C++ representation of any implementation of AderScene.cs
 */
class AderScene
{
public:
    /**
     * Create AderScene interface from the sharp class
     *
     * @param sClass SharpClass containing AderScene object
     */
    AderScene(AderSceneBase* base, Memory::reference<SharpClass> klass);

    ~AderScene();

    /**
     * Get the name of the AderScene
     */
    const std::string& getName() const;

    /**
     * Invoke ctor for this object
     */
    void invokeConstruct();

    /**
     * Checks if this scene is the starting scene and return true, false otherwise
     */
    bool isStartingScene();

    /**
     * Invoke LoadAssets for this object returns an exception if it occurred
     */
    Memory::reference<SharpException> invokeLoadAssets();

    /**
     * Returns all scene visuals
     */
    std::vector<Visual*> getVisuals();

    /**
     * Adds a visual to the scene
     */
    void addVisual(Visual* visual);

    /**
     * Removes the visual from the scene, used by the GameObject,
     * when the last GameObject from the visual is removed the 
     * visual is removed from the scene
     */
    void removeVisual(Visual* visual);

    /**
     * Creates a new game object
     */
    GameObject* newGameObject();
private:
    /// Class representation of this implementation
    Memory::reference<SharpClass> m_class;

    /// Constructor of AderScene
    Memory::reference<SharpMethod> m_constructor;

    /// LoadAssets method of AderScene
    Memory::reference<SharpMethod> m_loadAssets;

    /// C Instance field
    Memory::reference<SharpField> m_cInstance;

    /// Instance of this implementation
    MonoObject* m_pInstance;

    /// Objects of the scene
    std::vector<GameObject*> m_objects;

    /// Visuals that belong to this scene
    std::vector<Visual*> m_visuals;
};
