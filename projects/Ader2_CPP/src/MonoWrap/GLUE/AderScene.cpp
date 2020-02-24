#include "AderScene.h"

#include "Utility/Log.h"

AderScene::AderScene(AderSceneBase* base, Memory::reference<SharpClass> klass)
	: m_class(klass)
{
	// Create instance
	m_pInstance = m_class->createInstance();

	// Create constructor method
	m_constructor = m_class->getMethod(".ctor", "", false);

	// Create LoadAssets method
	m_loadAssets = base->LoadAssets->getAsVirtual(m_pInstance);

	// Create _CInstance field
	m_cInstance = m_class->getField("_CInstance");

	// Create audio listener
	m_pAudioListener = new AudioListener();
}

AderScene::~AderScene()
{
	// Delete game objects
	for (int i = 0; i < m_objects.size(); i++)
	{
		delete m_objects[i];
	}

	// Delete cameras
	for (int i = 0; i < m_cameras.size(); i++)
	{
		delete m_cameras[i];
	}

	// Delete audio listener
	delete m_pAudioListener;
}

void AderScene::update()
{
	// Update cameras
	for (Camera* cam : m_cameras)
	{
		cam->update();
	}
}

const std::string& AderScene::getName() const
{
	return m_class->getName();
}

void AderScene::invokeConstruct()
{
	// Invoke constructor and then link instnace
	m_constructor->invokeMethod(m_pInstance, nullptr);
	m_cInstance->setValue(m_pInstance, this);
}

bool AderScene::isStartingScene()
{
	// Check if the scene has the attribute StartScene
	for (Memory::reference<SharpAttribute>& attribute : m_class->getAttributes())
	{
		if (attribute->getClass()->getName() == "StartScene")
		{
			return true;
		}
	}

	return false;
}

Memory::reference<SharpException> AderScene::invokeLoadAssets()
{
	// Invoke method
	MonoObject* exception;
	m_loadAssets->invokeVirtualMethod(m_pInstance, nullptr, &exception);

	// Check for exception
	if (exception)
	{
		return new SharpException(exception);
	}
	else
	{
		return nullptr;
	}
}

std::vector<Visual*> AderScene::getVisuals()
{
	return m_visuals;
}

void AderScene::addVisual(Visual* visual)
{
	m_visuals.push_back(visual);
}

void AderScene::removeVisual(Visual* visual)
{
	// Get the visual iterator
	auto& it = std::find(m_visuals.begin(), m_visuals.end(), visual);

	if (it != m_visuals.end())
	{
		// Remove the visual
		m_visuals.erase(it);
	}
	else
	{
		LOG_WARN("Trying to remove already removed visual!");
	}
}

GameObject* AderScene::newGameObject()
{
	// Create new instance, add it to the objects list and return it
	GameObject* go = new GameObject(this);
	m_objects.push_back(go);
	return go;
}

Camera* AderScene::newCamera()
{
	// Create new instance, add it to the cameras list and return it
	Camera* cam = new Camera(this);
	m_cameras.push_back(cam);
	return cam;
}

Camera* AderScene::getActiveCamera()
{
	return m_pActiveCamera;
}

void AderScene::setActiveCamera(Camera* pCamera)
{
	m_pActiveCamera = pCamera;
}

AudioListener* AderScene::setAudioListener()
{
	m_pAudioListener->Altered = true;
	return m_pAudioListener;
}

AudioListener* AderScene::getAudioListener() const
{
	return m_pAudioListener;
}
