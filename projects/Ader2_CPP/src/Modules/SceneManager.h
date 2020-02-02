#pragma once

// SceneManager is part of the module system
#include "ModuleSystem/ModuleSystem.h"

// SceneManager manages scene instances
#include "MonoWrap/GLUE/AderScene.h"

/**
 * SceneManager provides the engine with the abilities to manipulate
 * scene objects and is also responsible for managing their lifetime.
 * SceneManager is also used to relay scene specific information to
 * the engine through specific messages
 *
 * MODULE
 * Messages:
 *  - AddScene
 *  - SetScene
 *  - LoadScene
 *  - LoadCurrentScene
 *  - ReloadSceneShaders
 *  - TransmitScenes
 *  - SystemUpdate
 *
 * Posts:
 *  - SceneChanged
 */
class SceneManager : public Module
{
public:
    // Inherited via Module
    virtual bool canShutdown() override;

    // Inherited via Module
    virtual void shutdown() override;

    // Inherited via Module
    virtual int onMessage(MessageBus::MessageType msg, MessageBus::DataType pData) override;

    /**
     * Sets the current scene
     */
    void setScene(const std::string& name);

    /**
     * Returns the current scene
     */
    Memory::reference<AderScene> getScene(const std::string& name);

    /**
     * Get current active scene
     */
    Memory::reference<AderScene> getCurrent();
private:
    /**
     * Reload current scene shaders. Used for debugging purposes
     *
     * @return 0 if there were no errors, otherwise error code
     */
    int reloadShaders();

    /**
     * Load a scene with the specified name
     *
     * @return 0 if there were no errors, otherwise error code
     */
    int loadScene(const std::string& name);

    /**
     * Set scene vector to the one received from the MonoManager
     */
    void transmitScenes(MessageBus::DataType pData);

    /**
     * Update scenes and scene manager
     */
    void update();
private:
    /// Starting scene of the game marked with StartScene attribute in cs
    Memory::reference<AderScene> m_startScene = nullptr;

    /// Current scene
    Memory::reference<AderScene> m_currentScene;

    /// All located ader scenes
    std::vector<Memory::reference<AderScene>> m_scenes;
};
