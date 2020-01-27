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
}

AderScene::~AderScene()
{
	for (int i = 0; i < m_visuals.size(); i++)
	{
		delete m_visuals[i];
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

Visual* AderScene::newVisual()
{
	// Create new visual, add it to the visuals vector and return
	Visual* visual = new Visual();
	visual->VAO = new VAO();
	visual->Shader = new Shader();
	m_visuals.push_back(visual);
	return visual;
}
