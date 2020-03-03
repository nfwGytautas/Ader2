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
			// Get numeric index in the vector
			int index = it - m_pVisual->Objects.begin();

			// Remove entry
			m_pVisual->Objects.erase(it);
			m_pVisual->Render.erase(m_pVisual->Render.begin() + index);

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
	m_pVisual->Render.push_back(true);
}

glm::mat4 GameObject::getTransformation()
{
	// Initialize empty matrix
	glm::mat4 transformation = glm::mat4(1);
	
	// Apply transform
	transformation = glm::translate(transformation, m_transform.Position);

	// Apply rotation
	transformation = glm::rotate(transformation, 
		glm::radians(m_transform.Rotation.x), 
		glm::vec3(1.0f, 0.0f, 0.0f));

	transformation = glm::rotate(transformation,
		glm::radians(m_transform.Rotation.y),
		glm::vec3(0.0f, 1.0f, 0.0f));

	transformation = glm::rotate(transformation,
		glm::radians(m_transform.Rotation.z),
		glm::vec3(0.0f, 0.0f, 1.0f));

	// Apply scaling
	transformation = glm::scale(transformation, m_transform.Scale);

	// Set transform update
	m_transformUpdate = false;

	// Return result
	return transformation;
}

bool GameObject::transformChanged()
{
	return m_transformUpdate;
}

const Transform& GameObject::getTransform() const
{
	return m_transform;
}

Transform& GameObject::setTransform()
{
	m_transformUpdate = true;
	return m_transform;
}

const glm::vec2& GameObject::getTexOffset() const
{
	return m_texOffset;
}

glm::vec2& GameObject::setTexOffset()
{
	m_offsetUpdate = true;
	return m_texOffset;
}

bool GameObject::offsetChanged()
{
	return m_offsetUpdate;
}

const glm::vec2& GameObject::getOffset(const glm::vec2& atlasDims)
{
	m_offsetUpdate = false;
	return glm::vec2(m_texOffset.x, atlasDims.y - 1 - m_texOffset.y) / atlasDims;
}
