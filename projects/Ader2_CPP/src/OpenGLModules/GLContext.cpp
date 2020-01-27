#include "GLContext.h"

// Engine messages
#include "Enums/Messages.h"

// Logging
#include "Utility/Log.h"

// Reading shader sources
#include "Utility/File.h"

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
    case Messages::msg_SceneChanged:
        changeScene(pData);
        return 0;
    }

    return 0;
}

void GLContext::toggleWireFrame(bool value)
{
    // Toggle Line and Fill modes
    if (value)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
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

    // Loop over each visual
    for (Visual* visual : m_activeScene->getVisuals())
    {
        // Bind the specific data
        visual->VAO->bind();
        visual->Shader->bind();

        visual->VAO->render();
    }

    // Swap window buffers
    glfwSwapBuffers(m_pRenderTarget);

    return 0;
}

void GLContext::changeScene(MessageBus::DataType pData)
{
    // Get scene
    m_activeScene = *static_cast<Memory::reference<AderScene>*>(pData);
}

VAO::VAO()
{
    // Create vertex array
    glGenVertexArrays(1, &m_idArray);
}

VAO::~VAO()
{
    // Delete the array
    glDeleteVertexArrays(1, &m_idArray);

    // Delete buffers
    deleteBuffer(m_idIndices);
    deleteBuffer(m_idVertices);
}

void VAO::render()
{
    // If indices array is set render with glDrawElements else glDrawArrays
    if (m_idIndices == 0)
    {
        glDrawArrays(GL_TRIANGLES, 0, m_renderCount);
    }
    else
    {
        glDrawElements(GL_TRIANGLES, m_renderCount, GL_UNSIGNED_INT, 0);
    }
}

void VAO::createIndiceBuffer(std::vector<unsigned int>& indices, bool dynamic)
{
    // Delete previous buffer
    deleteBuffer(m_idIndices);

    // Create buffer and add to buffers vector
    glGenBuffers(1, &m_idIndices);

    // Bind the buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_idIndices);

    // Add buffer data
    if (!dynamic)
    {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    }

    // Set render count
    m_renderCount = indices.size();
}

void VAO::createVerticesBuffer(std::vector<float>& vertices, bool dynamic)
{
    // Delete previous buffer
    deleteBuffer(m_idVertices);

    // Set render count
    if (m_idIndices != 0)
    {
        m_renderCount = vertices.size() / 3;
    }

    // Create buffer and add to buffers vector
    glGenBuffers(1, &m_idVertices);

    // Bind the buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_idVertices);

    // Add buffer data
    if (!dynamic)
    {
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    }

    // Assign attrib pointer
    glVertexAttribPointer(
        al_Vertices,
        3,
        GL_FLOAT,
        false, 
        3 * sizeof(float),
        (const void*)0
    );

    // Unbind buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VAO::bind() const
{
    // Binds the array
    glBindVertexArray(m_idArray);

    if (m_idVertices)
    {
        glEnableVertexAttribArray(al_Vertices);
    }
}

void VAO::deleteBuffer(unsigned int& buffer)
{
    // Check that the buffer is valid and then delete it
    if (buffer != 0)
    {
        glDeleteBuffers(1, &buffer);
    }
}

Shader::Shader()
{
}

Shader::~Shader()
{
    deleteShader();
}

void Shader::bind()
{
    glUseProgram(m_idShader);
}

void Shader::load()
{
    // Unbind the shader
    glUseProgram(0);

    // Delete shader and the reload the shader
    deleteShader();
    loadShader();
}

void Shader::deleteShader()
{
    glDeleteProgram(m_idShader);
}

void Shader::loadShader()
{
    // Info about shader compilation
    int success;
    char infoLog[512];

    // Vertex shader

    // Read vertex file
    std::string vertexSource = readFile(VertexSource).c_str();
    const char* vSource = vertexSource.c_str();

    // Create vertex shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // Compile vertex shader
    glShaderSource(vertexShader, 1, &vSource, NULL);
    glCompileShader(vertexShader);

    // Fragment shader

    // Read fragment file
    std::string fragmentSource = readFile(FragmentSource).c_str();
    const char* fSource = fragmentSource.c_str();

    // Create fragment shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Compile fragment shader
    glShaderSource(fragmentShader, 1, &fSource, NULL);
    glCompileShader(fragmentShader);

    // Check compilation results of shaders

    // Get compilation results
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    // If it failed get detailed info
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        LOG_ERROR("Failed to compile vertex shader:\n {0}", infoLog);
    }

    // Get compilation results
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    // If it failed get detailed info
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        LOG_ERROR("Failed to compile fragment shader:\n {0}", infoLog);
    }

    // Create shader program
    m_idShader = glCreateProgram();

    // Add shaders and link them
    glAttachShader(m_idShader, vertexShader);
    glAttachShader(m_idShader, fragmentShader);
    glLinkProgram(m_idShader);

    // Check if it linked correctly
    glGetProgramiv(m_idShader, GL_LINK_STATUS, &success);

    // If it failed get detailed info
    if (!success) 
    {
        glGetProgramInfoLog(m_idShader, 512, NULL, infoLog);
        LOG_ERROR("Failed to link shader:\n {0}", infoLog);
    }

    // Delete shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}
