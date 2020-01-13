#pragma once

// GLFWWindow is part of the module system
#include "ModuleSystem/ModuleSystem.h"

// OpenGL and GLFW includes
#include <glad/glad.h>
#include <GLFW/glfw3.h>

/**
 * This module provides a GLFW window interface, is responsible for creating
 * a window and returning input.
 *
 * MODULE
 * Messages:
 *  - Setup
 *  - CreateWindow
 *
 * Posts:
 *  - WindowCreated, sends GLFWWindow pointer
 */
class GLWindow : public Module
{
public:
    /**
     * Struct made for easier transfer of parameters for creating the window.
     * Not necessary to send this as long as the memory sent maps to this.
     */
    struct CreateWindowParams
    {
        /// Width of the window
        unsigned int width;

        /// Height of the window
        unsigned int height;

        /// Title of the window
        std::string title;
    };

public:
    // Inherited via Module
    virtual bool canShutdown() override;

    // Inherited via Module
    virtual void shutdown() override;

    // Inherited via Module
    virtual int onMessage(MessageBus::MessageType msg, MessageBus::DataType pData) override;

private:
    /**
     * Setup the GLFWWindow and return the status
     *
     * @return 0 if there were no errors, otherwise error code
     */
    int setup();

    /**
     * Create a GLFW window of specified width, height and Title
     *
     * @param width UInt width of window in pixels
     * @param height UInt height of window in pixels
     * @param title Title of the window
     *
     * @return 0 if there were no errors, otherwise error code
     */
    int createWindow(const unsigned int& width, const unsigned int& height, const std::string& title);

private:
    /// Window pointer
    GLFWwindow* m_pWindow;
};
