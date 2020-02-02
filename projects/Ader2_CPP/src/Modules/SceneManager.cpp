#include "SceneManager.h"

// Engine messages
#include "Enums/Messages.h"

// Logging
#include "Utility/Log.h"

// Find if
#include <algorithm>

bool SceneManager::canShutdown()
{
	return true;
}

void SceneManager::shutdown()
{
}

int SceneManager::onMessage(MessageBus::MessageType msg, MessageBus::DataType pData)
{
	switch (msg)
	{
	case Messages::msg_SetScene:
		setScene(*static_cast<const std::string*>(pData));
		this->postMessage(Messages::msg_SceneChanged, &m_currentScene);
		return 0;
	case Messages::msg_LoadScene:
		return loadScene(*static_cast<const std::string*>(pData));
	case Messages::msg_LoadCurrentScene:
		return loadScene(m_currentScene->getName());
	case Messages::msg_ReloadSceneShaders:
		return reloadShaders();
	case Messages::msg_TransmitScenes:
		transmitScenes(pData);
		return 0;
	case Messages::msg_SystemUpdate:
		update();
	}

	return 0;
}

void SceneManager::setScene(const std::string& name)
{
	Memory::reference<AderScene> scene = getScene(name);

	if (scene.valid())
	{
		m_currentScene = scene;
	}
}

Memory::reference<AderScene> SceneManager::getScene(const std::string& name)
{
	// Find scene
	auto& it = std::find_if(m_scenes.begin(), m_scenes.end(),
		[&](const Memory::reference<AderScene>& scene)
	{
		return scene->getName() == name;
	});

	// Check if the scene has been found
	if (it == m_scenes.end())
	{
		LOG_ERROR("Can't find scene {0}", name);
		return nullptr;
	}

	return *it;
}

Memory::reference<AderScene> SceneManager::getCurrent()
{
	return m_currentScene;
}

int SceneManager::reloadShaders()
{
	// Check if the active scene is set
	if (!m_currentScene.valid())
	{
		return 1;
	}

	for (Visual* visual : m_currentScene->getVisuals())
	{
		visual->Shader->load();
	}

	return 0;
}

int SceneManager::loadScene(const std::string& name)
{
	// Get the scene by name
	Memory::reference<AderScene> scene = getScene(name);

	// Check if such scene exists
	if (!scene.valid())
	{
		return 1;
	}

	// Load assets and check for exception
	Memory::reference<SharpException> ex = scene->invokeLoadAssets();

	if (ex.valid())
	{
		LOG_WARN("Scene '{0}' raised an exception while loading!\n Message: '{1}'\n Stack trace:\n{2}",
			scene->getName(), ex->getMessage(), ex->getStackTrace());
		return 1;
	}

	return 0;
}

void SceneManager::transmitScenes(MessageBus::DataType pData)
{
	m_scenes = *static_cast<std::vector<Memory::reference<AderScene>>*>(pData);

	// Find the start scene
	for (Memory::reference<AderScene>& scene : m_scenes)
	{
		if (scene->isStartingScene())
		{
			m_startScene = scene;
			m_currentScene = scene;
			this->postMessage(Messages::msg_SceneChanged, &m_currentScene);
			break;
		}
	}

	// Check if a start scene was specified
	if (!m_startScene.valid())
	{
		LOG_ERROR("Start scene not specified!");
	}
}

void SceneManager::update()
{
	// Update current scene
	m_currentScene->update();
}
