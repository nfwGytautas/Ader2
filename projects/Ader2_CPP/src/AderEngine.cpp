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
	m_inputInterface(new InputInterface()),
	m_sceneManager(new SceneManager()),
	m_assetManager(new AssetManager()),
	m_preRender(new PreRender())
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

Memory::reference<SceneManager> AderEngine::scenes()
{
	return m_sceneManager;
}

Memory::reference<AssetManager> AderEngine::assets()
{
	return m_assetManager;
}

Memory::reference<PreRender> AderEngine::preRender()
{
	return m_preRender;
}

std::vector<MessageBus::ModuleEntry> AderEngine::getModules()
{
	return
	{
		m_monoManager.as<Module>(),
		m_window.as<Module>(),
		m_glContext.as<Module>(),
		m_inputInterface.as<Module>(),
		m_sceneManager.as<Module>(),
		m_assetManager.as<Module>(),
		m_preRender.as<Module>(),
	};
}

MessageBus* AderEngine::getMBImplementation()
{
	return new AderMessageBus();
}
