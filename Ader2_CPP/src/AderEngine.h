#pragma once

// Engine is the ModuleSystem
#include "ModuleSystem/ModuleSystem.h"

// Engine messages
#include "Messages.h"

// Logger
#include "Utility/Log.h"

// Scripting module
#include "MonoWrap/MonoManager.h"


/**
 * The message bus that is used by the AderEngine
 */
class AderMessageBus : public MessageBus
{
public:
    // Inherited via MessageBus
    virtual bool canShutdown() override;

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
    AderEngine();

    /**
     * Get the reference to the script manager
     *
     * @return Memory::reference<MonoManager> of the engine's script manager
     */
    Memory::reference<MonoManager> scriptManager();

	// Inherited via ModuleSystem
	virtual std::vector<MessageBus::ModuleEntry> getModules() override;
	virtual MessageBus* getMBImplementation() override;

private:
    // Mono manager module
    Memory::reference<MonoManager> m_monoManager;
};
