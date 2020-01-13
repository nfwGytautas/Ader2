#include "GLContext.h"

// Engine messages
#include "Enums/Messages.h"

// Logging
#include "Utility/Log.h"

bool GLContext::canShutdown()
{
    return true;
}

void GLContext::shutdown()
{
}

int GLContext::onMessage(MessageBus::MessageType msg, MessageBus::DataType pData)
{
    switch (msg)
    {
    case Messages::msg_WindowCreated:
        return initContext(static_cast<GLFWwindow*>(pData));
    case Messages::msg_StateBundleCreated:
    {
        const StateBundle* bndl = static_cast<const StateBundle*>(pData);
        m_pWndState = &bndl->WndState;
        return 0;
    }
    case Messages::msg_WndStateUpdated:
        return wndStateUpdate();
    case Messages::msg_SystemRender:
        return render();
    }

    return 0;
}

int GLContext::initContext(GLFWwindow* pWindow)
{
    // Set the context to the newly created window
    glfwMakeContextCurrent(pWindow);

    // Initialize Glad after the context is set
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        LOG_ERROR("Failed to initialize Glad!");
        return 1;
    }

    // Set the render target
    m_pRenderTarget = pWindow;

    return 0;
}

int GLContext::wndStateUpdate()
{
    if (m_pWndState->resized)
    {
        // Set the view port
        glViewport(0, 0, m_pWndState->width, m_pWndState->height);
    }

    return 0;
}

int GLContext::render()
{
    //  Clear the color of the screen
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);


    // Swap window buffers
    glfwSwapBuffers(m_pRenderTarget);

    return 0;
}
