#include "InputInterface.h"

// GLFW
#include <GLFW/glfw3.h>

// Engine messages
#include "Enums/Messages.h"

InputInterface* InputInterface::ms_pStaticThis = nullptr;

InputInterface::InputInterface()
	: m_actualState(), m_msgState()
{
	ms_pStaticThis = this;
}

bool InputInterface::canShutdown()
{
	return true;
}

void InputInterface::shutdown()
{
}

int InputInterface::onMessage(MessageBus::MessageType msg, MessageBus::DataType pData)
{
	switch (msg)
	{
	case Messages::msg_Setup:
		return setup();
	case Messages::msg_WindowCreated:
		return initWindowInput(pData);
	case Messages::msg_SystemUpdate:
		return update();
	}

	return 0;
}

const WindowState& InputInterface::getWndState() const
{
	return m_actualState.WndState;
}

const KeyboardState& InputInterface::getKeyState() const
{
	return m_actualState.KeyState;
}

int InputInterface::setup()
{
	// Send states
	this->postMessage(Messages::msg_StateBundleCreated, &m_msgState);

	return 0;
}

int InputInterface::initWindowInput(MessageBus::DataType pWindow)
{
	// Cast to GLFW window
	GLFWwindow* pGLFW = static_cast<GLFWwindow*>(pWindow);

	// Get size of the window
	glfwGetWindowSize(pGLFW, &m_actualState.WndState.width, &m_actualState.WndState.height);

	// Make sure to init the engine with the initial window size
	m_actualState.WndState.resized = true;
	m_msgState = m_actualState;
	this->postMessage(Messages::msg_WndStateUpdated);

	// Subscribe callbacks

	// Framebuffer size
	glfwSetFramebufferSizeCallback(pGLFW, [](GLFWwindow* window, int width, int height)
	{
		ms_pStaticThis->framebufferResize(width, height);
	});

	// Window close
	glfwSetWindowCloseCallback(pGLFW, [](GLFWwindow* window)
	{
		ms_pStaticThis->windowClose();
	});

	// Key callback
	glfwSetKeyCallback(pGLFW, [](GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		ms_pStaticThis->keyInput(key, action, mods);
	});

	return 0;
}

int InputInterface::update()
{
	// Reset window state flags
	m_actualState.WndState.resized = false;

	// Poll inputs
	glfwPollEvents();

	// Equalize states
	m_msgState.WndState = m_actualState.WndState;
	m_msgState.KeyState = m_actualState.KeyState;

	// Check if the window state changed and post the update if it did
	if (m_wndStateChanged)
	{
		this->postMessage(Messages::msg_WndStateUpdated);
		m_wndStateChanged = false;
	}

	// Check if the keyboard state changed and post the update if it did
	if (m_keyStateChanged)
	{
		this->postMessage(Messages::msg_KeyStateUpdated);
		m_keyStateChanged = false;
	}

	return 0;
}

void InputInterface::framebufferResize(int width, int height)
{
	m_actualState.WndState.width = width;
	m_actualState.WndState.height = height;
	m_actualState.WndState.resized = true;
	m_wndStateChanged = true;
}

void InputInterface::windowClose()
{
	m_actualState.WndState.closed = true;
	m_wndStateChanged = true;
}

void InputInterface::keyInput(int key, int action, int mods)
{
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		m_actualState.KeyState.Keys[key] = true;
	}
	else
	{
		m_actualState.KeyState.Keys[key] = false;
	}

	m_keyStateChanged = true;
}
