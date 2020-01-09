#include "AderEngine.h"

bool AderMessageBus::canShutdown()
{
	// Currently the message bus is not blocked and can shut down whenever
	return true;
}

void AderMessageBus::postMessage(MessageType msg, DataType pData)
{
	// Filter out messages

	// PASS

	// Send the message
	for (const Memory::reference<Module>& module : prtc_modules)
	{
		module->onMessage(msg, pData);
	}
}

AderEngine::AderEngine()
	: m_monoManager(new MonoManager())
{
	// Initialize logger
	Log::init();

	// Initialize the module system
	this->moduleSysInit();

	// Post setup message
	this->postMessage(Messages::Setup);
}

Memory::reference<MonoManager> AderEngine::scriptManager()
{
	return m_monoManager;
}

std::vector<MessageBus::ModuleEntry> AderEngine::getModules()
{
	return 
	{
		m_monoManager.as<Module>()
	};
}

MessageBus* AderEngine::getMBImplementation()
{
	return new AderMessageBus();
}
