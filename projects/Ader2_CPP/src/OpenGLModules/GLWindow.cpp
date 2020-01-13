#include "GLWindow.h"

// Engine messages
#include "Enums/Messages.h"

// Logging
#include "Utility/Log.h"

bool GLWindow::canShutdown()
{
	// Don't block the window
	return true;
}

void GLWindow::shutdown()
{
	// Shutdown glfw
	glfwTerminate();
}

int GLWindow::onMessage(MessageBus::MessageType msg, MessageBus::DataType pData)
{
	switch (msg)
	{
	case Messages::msg_Setup:
		return setup();

	case Messages::msg_CreateWindow:
		const CreateWindowParams* params = static_cast<const CreateWindowParams*>(pData);
		return createWindow(params->width, params->height, params->title);
	}

	return 0;
}

int GLWindow::setup()
{
	// Initialize GLFW
	if (glfwInit() != GLFW_TRUE)
	{
		LOG_ERROR("Failed to initialize GLFW!");
		return 1;
	}

	// Set OpenGL version to 4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// Core profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	return 0;
}

int GLWindow::createWindow(const unsigned int& width, const unsigned int& height, const std::string& title)
{
	// Create window
	m_pWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

	if (!m_pWindow)
	{
		LOG_ERROR("Failed to create GLFW window!");
		return 1;
	}

	// Post message that a new window was created
	this->postMessage(Messages::msg_WindowCreated, m_pWindow);

	return 0;
}
