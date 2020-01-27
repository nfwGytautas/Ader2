#pragma once

// Engine is the ModuleSystem
#include "ModuleSystem/ModuleSystem.h"

// Engine messages
#include "Enums/Messages.h"

// Logger
#include "Utility/Log.h"

// Scripting module
#include "MonoWrap/MonoManager.h"

// Window module
#include "OpenGLModules/GLWindow.h"

// Context module
#include "OpenGLModules/GLContext.h"

// Input interface module
#include "Modules/InputInterface.h"

// Scene manager module
#include "Modules/SceneManager.h"


/**
 * The message bus that is used by the AderEngine
 * For now its single threaded
 */
class AderMessageBus : public MessageBus
{
public:
    // Inherited via MessageBus
    virtual bool canShutdown() override;

    // Inherited via MessageBus
    virtual void shutdown() override;

    // Inherited via MessageBus
    virtual void postMessage(MessageType msg, DataType pData) override;
private:
};


/**
 * Main class of the engine
 */
class AderEngine : public ModuleSystem
{
public:
    /**
     * Create the engine object, during this the logging system will be initialized
     * as well as the entire ModuleSystem
     */
    AderEngine();

    /**
     * Return True if the window has closed and the program should end
     */
    bool shouldClose();

    /**
     * Get the reference to the script manager module
     *
     * @return Memory::reference<MonoManager> of the engine's script manager
     */
    Memory::reference<MonoManager> scriptManager();

    /**
     * Get the reference to the window module
     *
     * @return Memory::reference<GLWindow> of the engine's window module
     */
    Memory::reference<GLWindow> window();

    /**
     * Get the reference to the context module
     *
     * @return Memory::reference<GLContext> of the engine's context module
     */
    Memory::reference<GLContext> context();

    /**
     * Get the reference to the input interface module
     *
     * @return Memory::reference<InputInterface> of the engine's input interface module
     */
    Memory::reference<InputInterface> input();

    /**
     * Get the reference to the scene manager module
     *
     * @return Memory::reference<SceneManager> of the engine's scene manager module
     */
    Memory::reference<SceneManager> scenes();

	// Inherited via ModuleSystem
	virtual std::vector<MessageBus::ModuleEntry> getModules() override;
	virtual MessageBus* getMBImplementation() override;

private:
    Memory::reference<MonoManager> m_monoManager;
    Memory::reference<GLWindow> m_window;
    Memory::reference<GLContext> m_glContext;
    Memory::reference<InputInterface> m_inputInterface;
    Memory::reference<SceneManager> m_sceneManager;
};
