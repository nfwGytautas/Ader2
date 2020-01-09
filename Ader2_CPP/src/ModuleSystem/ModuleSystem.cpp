#include "ModuleSystem.h"

#include "Utility/Log.h"

void MessageBus::initModules(std::vector<Memory::reference<Module>> modules)
{
	prtc_modules = modules;
	for (auto& module : prtc_modules)
	{
		module->setMsgBus(this);
	}
}

void Module::postMessage(MessageBus::MessageType msg, MessageBus::DataType pData)
{
	if (m_pMsgBus)
	{
		m_pMsgBus->postMessage(msg, pData);
	}
}

void Module::setMsgBus(MessageBus* pMsgBus)
{
	m_pMsgBus = pMsgBus;
}

bool ModuleSystem::canShutdown()
{
	bool safeToShutdown = true;
	for (Memory::reference<Module> module : getModules())
	{
		safeToShutdown = module->canShutdown();

		if (!safeToShutdown)
		{
			return false;
		}
	}

	safeToShutdown = m_MsgBus->canShutdown();
	return safeToShutdown;
}

void ModuleSystem::moduleSysInit()
{
	m_MsgBus = getMBImplementation();

	if (m_MsgBus.valid())
	{
		m_MsgBus->initModules(getModules());
	}
	else
	{
		LOG_ERROR("Message bus implementation is invalid!");
	}
}

void ModuleSystem::postMessage(MessageBus::MessageType msg, MessageBus::DataType pData)
{
	if (m_MsgBus.valid())
	{
		m_MsgBus->postMessage(msg, pData);
	}
}
