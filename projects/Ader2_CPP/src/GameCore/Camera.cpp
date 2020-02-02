#include "Camera.h"

Camera::Camera(AderScene* pScene)
	: m_pScene(pScene)
{
}

void Camera::update()
{
	// If the camera doesn't need updating do nothing
	if (!m_needsUpdate)
	{
		return;
	}

	// Up vector for the camera
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	// Type alias
	float pitch = m_rotation.x;
	float yaw = m_rotation.y;
	float roll = m_rotation.z;

	// Calculate the direction of the camera from the rotation
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	// Normalize direction to get camera front vector
	glm::vec3 front = glm::normalize(direction);

	// Calculate view matrix
	m_viewMatrix = glm::lookAt(
		m_position, 
		m_position + front,
		up);

	// Remove update flag
	m_needsUpdate = false;
}

const glm::vec3& Camera::getPosition() const
{
	return m_position;
}

void Camera::setPosition(const glm::vec3& value)
{
	m_needsUpdate = true;
	m_position = value;
}

const glm::vec3& Camera::getRotation() const
{
	return m_rotation;
}

void Camera::setRotation(const glm::vec3& value)
{
	m_needsUpdate = true;
	m_rotation = value;
}

const glm::mat4& Camera::getViewMatrix() const
{
	return m_viewMatrix;
}
