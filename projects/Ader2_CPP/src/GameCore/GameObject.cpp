#include "GameObject.h"

// Visual and scene
#include "MonoWrap/GLUE/AderScene.h"

// Logging
#include "Utility/Log.h"

GameObject::GameObject(AderScene* scene)
	: m_pScene(scene)
{
}

Visual* GameObject::getVisual()
{
	return m_pVisual;
}

AderScene* GameObject::getScene()
{
	return m_pScene;
}

void GameObject::setVisual(Visual* visual)
{
	// Check if the game object had a visual before
	if (m_pVisual != nullptr)
	{
		// Check if the previous visual had this game object which it should have
		auto& it = std::find(m_pVisual->Objects.begin(), m_pVisual->Objects.end(), this);
		if (it == m_pVisual->Objects.end())
		{
			LOG_WARN("Visual didn't have a game object!");
		}
		else
		{
			// Remove entry
			m_pVisual->Objects.erase(it);

			// Check if there are any game objects left
			if (m_pVisual->Objects.size() <= 0)
			{
				// Remove the visual from the scene if there aren't any game objects left
				m_pScene->removeVisual(m_pVisual);
			}
		}
	}

	// Assign new visual
	m_pVisual = visual;
	
	// Check if there are any game object in the visual
	if (m_pVisual->Objects.size() <= 0)
	{
		// Add the visual since it wasn't added before
		m_pScene->addVisual(m_pVisual);
	}

	// Add this game object to the visual batch
	m_pVisual->Objects.push_back(this);
}
