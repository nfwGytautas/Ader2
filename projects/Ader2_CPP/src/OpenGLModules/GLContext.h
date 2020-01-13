#pragma once

// GLContext is part of the module system
#include "ModuleSystem/ModuleSystem.h"

// OpenGL includes
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Window state structure
#include "Modules/InputInterface.h"

/**
 * This module provides a OpenGL based hardware acceleration for the engine
 *
 * MODULE
 * Messages:
 *  - WndStateCreated
 *  - WndStateUpdated
 *  - WindowCreated
 *  - SystemRender
 */
class GLContext : public Module
{
public:
    // Inherited via Module
    virtual bool canShutdown() override;

    // Inherited via Module
    virtual void shutdown() override;

    // Inherited via Module
    virtual int onMessage(MessageBus::MessageType msg, MessageBus::DataType pData) override;

private:
    /**
     * Initialize OpenGL context with the provided GLFWWindow
     *
     * @param pWindow Pointer to GLFWwindow structure
     *
     * @return 0 if there were no errors, otherwise error code
     */
    int initContext(GLFWwindow* pWindow);

    /**
     * Called when ever the window state has been updated
     *
     * @return 0 if there were no errors, otherwise error code
     */
    int wndStateUpdate();

    /**
     * Called when ever the render message is received
     *
     * @return 0 if there were no errors, otherwise error code
     */
    int render();
private:
    /// Pointer to the window that the context is rendering to
    GLFWwindow* m_pRenderTarget;

    /// Window state received from the InputInterface
    const WindowState* m_pWndState;
};
