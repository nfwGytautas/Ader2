#include "AderEngine.h"

bool AderMessageBus::canShutdown()
{
	// Currently the message bus is not blocked and can shut down whenever
	return true;
}

void AderMessageBus::shutdown()
{
	// Shutdown each module
	for (const Memory::reference<Module>& module : prtc_modules)
	{
		// Check if the module is ready to shutdown
		if (!module->canShutdown())
		{
			// Log warning that the module wasn't ready to shutdown
			LOG_WARN("Module being shutdown without being ready!");
		}

		module->shutdown();
	}
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
	: 
	m_monoManager(new MonoManager()), 
	m_window(new GLWindow()), 
	m_glContext(new GLContext()),
	m_inputInterface(new InputInterface())
{
	// Initialize logger
	Log::init();

	// Initialize the module system
	this->moduleSysInit();
}

bool AderEngine::shouldClose()
{
	return m_inputInterface->getWndState().closed;
}

Memory::reference<MonoManager> AderEngine::scriptManager()
{
	return m_monoManager;
}

Memory::reference<GLWindow> AderEngine::window()
{
	return m_window;
}

Memory::reference<GLContext> AderEngine::context()
{
	return m_glContext;
}

Memory::reference<InputInterface> AderEngine::input()
{
	return m_inputInterface;
}

std::vector<MessageBus::ModuleEntry> AderEngine::getModules()
{
	return
	{
		m_monoManager.as<Module>(),
		m_window.as<Module>(),
		m_glContext.as<Module>(),
		m_inputInterface.as<Module>(),
	};
}

MessageBus* AderEngine::getMBImplementation()
{
	return new AderMessageBus();
}
