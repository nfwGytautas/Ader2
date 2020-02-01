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

    // OpenGL error callback
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(
        [](
            GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity,
            GLsizei length,
            const GLchar* message,
            const void* userParam)
    {

        if (type == GL_DEBUG_TYPE_ERROR)
        {
            LOG_ERROR("OpenGL error: \n\
                Source: {0}\n\
                Message: {1}", source, message);
        }

    }, 0);

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
        
        // Create instance buffer
        visual->VAO->createInstanceBuffer(visual->Transforms, true);
        
        // Render using instancing
        visual->VAO->renderInstance(visual->RenderCount);
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

    // Assign VBO types
    m_idIndices.Type = GL_ELEMENT_ARRAY_BUFFER;
    m_idVertices.Type = GL_ARRAY_BUFFER;
    m_idTexCoords.Type = GL_ARRAY_BUFFER;
    m_idInstance.Type = GL_ARRAY_BUFFER;
}

VAO::~VAO()
{
    // Delete the array
    glDeleteVertexArrays(1, &m_idArray);

    // Delete buffers
    deleteBuffer(m_idIndices);
    deleteBuffer(m_idVertices);
    deleteBuffer(m_idTexCoords);
    deleteBuffer(m_idInstance);
}

void VAO::render()
{
    // If indices array is set render with glDrawElements else glDrawArrays
    if (m_idIndices.ID == 0)
    {
        glDrawArrays(GL_TRIANGLES, 0, m_renderCount);
    }
    else
    {
        glDrawElements(GL_TRIANGLES, m_renderCount, GL_UNSIGNED_INT, 0);
    }
}

void VAO::renderInstance(unsigned int count)
{
    // If indices array is set render with glDrawElements else glDrawArrays
    if (m_idIndices.ID == 0)
    {
        glDrawArraysInstanced(GL_TRIANGLES, 0, m_renderCount, count);
    }
    else
    {
        glDrawElementsInstanced(GL_TRIANGLES, m_renderCount, GL_UNSIGNED_INT, 0, count);
    }
}

void VAO::createIndiceBuffer(std::vector<unsigned int>& indices, bool dynamic)
{
    if (setupBuffer(
        m_idIndices,
        dynamic,
        sizeof(unsigned int),
        indices.size(),
        indices.data()))
    {
        // No need for attributes
    }

    // Set render count
    m_renderCount = indices.size();
}

void VAO::createVerticesBuffer(std::vector<float>& vertices, bool dynamic)
{
    if (setupBuffer(
        m_idVertices,
        dynamic,
        sizeof(float),
        vertices.size(),
        vertices.data()))
    {
        // Assign attrib pointer
        glVertexAttribPointer(
            al_Vertices,
            3,
            GL_FLOAT,
            false,
            3 * sizeof(float),
            (const void*)0
        );
    }

    // Set render count but don't override it if there is an indices buffer
    if (m_idVertices.ID != 0 && m_idIndices.ID == 0)
    {
        m_renderCount = vertices.size() / 3;
    }
}

void VAO::createUVBuffer(std::vector<float>& texCoords, bool dynamic)
{
    if (setupBuffer(
        m_idTexCoords, 
        dynamic, 
        sizeof(float), 
        texCoords.size(), 
        texCoords.data()))
    {
        // Assign attrib pointer
        glVertexAttribPointer(
            al_TexCoord,
            2,
            GL_FLOAT,
            false,
            2 * sizeof(float),
            (const void*)0
        );
    }
}

void VAO::createInstanceBuffer(std::vector<glm::mat4>& transforms, bool dynamic)
{
    if (setupBuffer(
        m_idInstance,
        dynamic,
        sizeof(glm::mat4),
        transforms.size(),
        transforms.data()))
    {
        // Since OpenGL vertex attribute max size is vec4 in order
        // to have a mat4 we need 4 attributes since a mat4 is just
        // 4 vec4

        glEnableVertexAttribArray(al_Instance0);
        glVertexAttribPointer(al_Instance0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
                                                                   
        glEnableVertexAttribArray(al_Instance1);                   
        glVertexAttribPointer(al_Instance1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
                                                                   
        glEnableVertexAttribArray(al_Instance2);                   
        glVertexAttribPointer(al_Instance2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
                                                                   
        glEnableVertexAttribArray(al_Instance3);                   
        glVertexAttribPointer(al_Instance3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

        // Since this is used for instancing we define their divisors
        // to be 1 each meaning it will be incremented by 1 each instance draw
        glVertexAttribDivisor(al_Instance0, 1);
        glVertexAttribDivisor(al_Instance1, 1);
        glVertexAttribDivisor(al_Instance2, 1);
        glVertexAttribDivisor(al_Instance3, 1);
    }
}

void VAO::bind() const
{
    ADER_ASSERT(m_idArray != 0, "Trying to bind invalid vertex array");

    // Binds the array
    glBindVertexArray(m_idArray);

    // Enable vertex attribute arrays
    if (m_idVertices.ID)
    {
        glEnableVertexAttribArray(al_Vertices);
    }

    if (m_idTexCoords.ID)
    {
        glEnableVertexAttribArray(al_TexCoord);
    }

    if (m_idInstance.ID)
    {
        glEnableVertexAttribArray(al_Instance0);
        glEnableVertexAttribArray(al_Instance1);
        glEnableVertexAttribArray(al_Instance2);
        glEnableVertexAttribArray(al_Instance3);
    }
}

bool VAO::setupBuffer(VBO& buffer, bool dynamic, size_t eSize, size_t eCount, void* pData)
{
    // Bind this VAO
    bind();

    // Check if the buffer is created first
    if (buffer.ID == 0)
    {
        // Create buffer
        createBuffer(buffer);

        // Bind the buffer
        glBindBuffer(buffer.Type, buffer.ID);

        // Set the type of the buffer object
        buffer.Dynamic = dynamic;

        // Allocate buffer
        allocBuffer(buffer, eSize, eCount, pData);

        // Set up attributes
        return true;
    }
    else
    {
        // Bind the buffer
        glBindBuffer(buffer.Type, buffer.ID);
    }

    // If there is mismatch between dynamic and static buffer we
    // have to reallocate it with the correct format
    if (buffer.Dynamic != dynamic)
    {
        // Set the type of the buffer object
        buffer.Dynamic = dynamic;

        // Allocate buffer
        allocBuffer(buffer, eSize, eCount, pData);

        // VBO was created so attributes should have been created already
        return false;
    }

    // Check if the buffer was dynamic or not if it was we don't
    // want to delete the buffer but just change it's data
    if (buffer.Dynamic)
    {
        // This will modify the data of the buffer or reallocate it
        // if it is too small
        modifyBuffer(buffer, eSize, eCount, pData);

        // VBO was created so attributes should have been created already
        return false;
    }
    else
    {
        // Allocate new static buffer memory
        allocBuffer(buffer, eSize, eCount, pData);

        // VBO was created so attributes should have been created already
        return false;
    }

    // Should not reach this point
    return false;
}

void VAO::deleteBuffer(VBO& buffer)
{
    // Check that the buffer is valid and then delete it
    if (buffer.ID != 0)
    {
        glDeleteBuffers(1, &buffer.ID);
        buffer.Size = 0;
    }
}

void VAO::createBuffer(VBO& vbo)
{
    // Create buffer and add to buffers vector
    glGenBuffers(1, &vbo.ID);
}

void VAO::allocBuffer(VBO& vbo, size_t eSize, size_t eCount, void* pInitData)
{
    // Add buffer data
    glBufferData(vbo.Type, eCount * eSize, pInitData, vbo.Dynamic ? GL_STREAM_DRAW : GL_STATIC_DRAW);
    vbo.Size = eCount * eSize;
}

void VAO::modifyBuffer(VBO& vbo, size_t eSize, size_t eCount, void* pData)
{
    // Check size of buffer
    if (vbo.Size < eSize * eCount)
    {
        // Allocate buffer since the previous one was too small
        allocBuffer(vbo, eSize, eCount, pData);

        // Do nothing else
        return;
    }

    // Map buffer and get it's contents pointer
    void* ptr = glMapBuffer(vbo.Type, GL_WRITE_ONLY);

    // Copy new data and don't change the size variable
    memcpy(ptr, pData, eSize * eCount);

    // Unmap the buffer
    glUnmapBuffer(vbo.Type);
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
