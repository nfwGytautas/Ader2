#pragma once

// SceneManager is part of the module system
#include "ModuleSystem/ModuleSystem.h"

// SceneManager manages scene instances
#include "MonoWrap/GLUE/AderScene.h"

/**
 * PreRender module is used to determine which game objects should be
 * rendered and updated, and then does the necessary updates
 *
 * MODULE
 * Messages:
 *  - SceneChanged
 *  - PreRender
 *
 * Posts:
 *
 */
class PreRender : public Module
{
public:
    // Inherited via Module
    virtual bool canShutdown() override;

    // Inherited via Module
    virtual void shutdown() override;

    // Inherited via Module
    virtual int onMessage(MessageBus::MessageType msg, MessageBus::DataType pData) override;
private:
    /**
     * Set scene vector to the one received from the MonoManager
     */
    void sceneChanged(MessageBus::DataType pData);

    /**
     * Utility method
     */
    void preRender();

    /**
     * Updates the transformations of the necessary game objects of
     * the specified visual
     */
    void updateTransforms(Visual* visual);

    /**
     * Updates the texture offset values of the necessary game objects
     * of the specified visual
     */
    void updateTexOffsets(Visual* visual);
private:
    /// Current scene
    Memory::reference<AderScene> m_currentScene;
};
