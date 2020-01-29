#include "GLContext.h"

// Engine messages
#include "Enums/Messages.h"

// Logging
#include "Utility/Log.h"

// Reading shader sources and texture files
#include "Utility/File.h"

// Assert
#include "Defs.h"



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

        // Bind textures to their slots
        for (auto& it : visual->Textures)
        {
            it.second->bind(it.first);
        }

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
    deleteBuffer(m_idTexCoords);
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
    if (m_idVertices != 0)
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

void VAO::createUVBuffer(std::vector<float>& texCoords, bool dynamic)
{
    // Delete previous buffer
    deleteBuffer(m_idTexCoords);

    // Create buffer and add to buffers vector
    glGenBuffers(1, &m_idTexCoords);

    // Bind the buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_idTexCoords);

    // Add buffer data
    if (!dynamic)
    {
        glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(float), texCoords.data(), GL_STATIC_DRAW);
    }

    // Assign attrib pointer
    glVertexAttribPointer(
        al_TexCoord,
        2,
        GL_FLOAT,
        false,
        2 * sizeof(float),
        (const void*)0
    );

    // Unbind buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VAO::bind() const
{
    ADER_ASSERT(m_idArray != 0, "Trying to bind invalid vertex array");

    // Binds the array
    glBindVertexArray(m_idArray);

    // Enable vertex attribute arrays
    if (m_idVertices)
    {
        glEnableVertexAttribArray(al_Vertices);
    }

    if (m_idTexCoords)
    {
        glEnableVertexAttribArray(al_TexCoord);
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
    ADER_ASSERT(m_idShader != 0, "Trying to bind invalid shader");
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

Texture::Texture()
{
}

Texture::~Texture()
{
    // Delete the texture
    deleteTexture();
}

void Texture::bind(unsigned int slot)
{
    ADER_ASSERT(m_idTexture != 0, "Trying to bind invalid texture");
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_idTexture);
}

void Texture::load()
{
    // Delete current texture if it exists
    deleteTexture();

    // Load the texture
    loadTexture();
}

void Texture::deleteTexture()
{
    // Check that the texture is valid and then delete it
    if (m_idTexture != 0)
    {
        glDeleteTextures(1, &m_idTexture);
    }
}

void Texture::loadTexture()
{
    // Load texture
    Memory::reference<ImageFileContents> image = readImage(Source);

    if (!image.valid())
    {
        LOG_WARN("Texture couldn't be created!");
        return;
    }

    // Generate texture
    glGenTextures(1, &m_idTexture);

    // Bind the texture
    glBindTexture(GL_TEXTURE_2D, m_idTexture);

    // Texture parameters

    // Repeat textures when the object is to big
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Interpolate final color
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Create texture
    if (image->BPP == 3)
    {
        // Create RGB texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->Width, image->Height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->Buffer);
    }
    else if (image->BPP == 4)
    {
        // Create RGBA texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->Width, image->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->Buffer);
    }

    // Generate mip maps
    glGenerateMipmap(GL_TEXTURE_2D);
}
