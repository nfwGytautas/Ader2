#include "GLContext.h"

// Engine messages
#include "Enums/Messages.h"

// Logging
#include "Utility/Log.h"

// Reading shader sources and texture files
#include "Utility/File.h"

// Assert
#include "Defs.h"

#include <thread>

GLContext::GLContext()
{
}

bool GLContext::canShutdown()
{
    return true;
}

void GLContext::shutdown()
{
    // Delete UBOs
    if (m_pubMatrices)
    {
        delete m_pubMatrices;
    }

    if (m_pubTextureDetail)
    {
        delete m_pubTextureDetail;
    }

    // Destroy the context
    alcMakeContextCurrent(NULL);
    alcDestroyContext(m_pAudioContext);

    // Close the device
    alcCloseDevice(m_pAudioDevice);
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
    case Messages::msg_SystemUpdate:
        update();
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

    // Create OpenAL device
    m_pAudioDevice = alcOpenDevice(NULL);

    // Load ALC function pointers
    ALboolean enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
    if (enumeration == AL_FALSE)
    {
        LOG_ERROR("OpenAL alc extension not found!");
        return 1;
    }
    else
    {
        // Create context
        m_pAudioContext = alcCreateContext(m_pAudioDevice, NULL);
        if (!alcMakeContextCurrent(m_pAudioContext))
        {
            LOG_ERROR("OpenAL context initialization failed!");
        }

        // Reset OpenAL error stack
        alGetError();
        ALenum alerror = alGetError();
        if (alerror != AL_NO_ERROR)
        {
            LOG_ERROR("OpenAL error '{0}'!", alerror);
            return 2;
        }
    }

    // Set the render target
    m_pRenderTarget = pWindow;

    // We can now create our UBO's
    m_pubMatrices = new UniformBuffer(UniformBuffer::bp_Mat);
    m_pubTextureDetail = new UniformBuffer(UniformBuffer::bp_TexDetail);

    // Create new audio listener
    m_pAudioListener = new AudioListener();

    return 0;
}

int GLContext::wndStateUpdate()
{
    // Check for resizing
    if (m_pWndState->resized)
    {
        wndResized();
    }

    return 0;
}

void GLContext::wndResized()
{
    // Set the view port
    glViewport(0, 0, m_pWndState->width, m_pWndState->height);

    if (m_pWndState->width != 0 && m_pWndState->height != 0)
    {
        // Recalculate projection matrix
        m_projection = glm::perspective(
            glm::radians(m_settings.FoV),
            (float)m_pWndState->width / (float)m_pWndState->height,
            m_settings.NearPlane,
            m_settings.FarPlane);

        // Recalculate orthographic matrix
        m_orthographic = glm::ortho(0.0f, (float)m_pWndState->width, 
            0.0f, (float)m_pWndState->height);
    }
}

int GLContext::render()
{
    //  Clear the color of the screen
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Construct matrices uniform buffer
    m_pubMatrices->bind();

    // Set the projection matrix
    m_pubMatrices->setSubData(0, sizeof(glm::mat4), glm::value_ptr(m_projection));

    // Set the view matrix
    m_pubMatrices->setSubData(sizeof(glm::mat4), sizeof(glm::mat4), (void*)glm::value_ptr(m_activeScene->getActiveCamera()->getViewMatrix()));

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

        // Bing the texture detail UBO
        m_pubTextureDetail->bind();
        
        // Set the atlas dimensions
        m_pubTextureDetail->setSubData(0, sizeof(glm::vec2), (void*)glm::value_ptr(visual->AtlasDims));

        // Create instance buffer
        visual->VAO->createInstanceBuffer(visual->Transforms, true);

        // Create offset buffer
        visual->VAO->createOffsetBuffer(visual->Offsets, true);

        // Render using instancing
        visual->VAO->renderInstance(visual->RenderCount);
    }

    // Set the orthographic matrix
    m_pubMatrices->bind();
    m_pubMatrices->setSubData(0, sizeof(glm::mat4), glm::value_ptr(m_orthographic));

    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Loop for each UI(Text) element
    for (Text* text : m_activeScene->getUI())
    {
        text->render();
    }

    // Swap window buffers
    glfwSwapBuffers(m_pRenderTarget);

    return 0;
}

void GLContext::changeScene(MessageBus::DataType pData)
{
    // Get scene
    m_activeScene = *static_cast<Memory::reference<AderScene>*>(pData);
    m_pAudioListener = m_activeScene->getAudioListener();
}

void GLContext::update()
{
    if (m_pAudioListener && m_pAudioListener->Altered)
    {
        // Set listener position
        alListener3f(AL_POSITION, 
            m_pAudioListener->Position.x,
            m_pAudioListener->Position.y,
            m_pAudioListener->Position.z);

        // Set volume
        alListeneri(AL_GAIN, 
            m_pAudioListener->Volume);

        // Set orientation
        alListenerfv(AL_ORIENTATION, static_cast<float*>((void*)&m_pAudioListener->Orientation));

        // Set flag
        m_pAudioListener->Altered = false;
    }
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
    m_idOffsets.Type = GL_ARRAY_BUFFER;
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
    deleteBuffer(m_idOffsets);
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

void VAO::createOffsetBuffer(std::vector<glm::vec2>& offsets, bool dynamic)
{
    if (setupBuffer(
        m_idOffsets,
        dynamic,
        sizeof(glm::vec2),
        offsets.size(),
        offsets.data()))
    {
        glEnableVertexAttribArray(al_TexOffset);
        glVertexAttribPointer(al_TexOffset, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
        
        // Instances will change the texture atlas
        glVertexAttribDivisor(al_TexOffset, 1);
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

    if (m_idOffsets.ID)
    {
        glEnableVertexAttribArray(al_TexOffset);
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

void Texture::load(unsigned int width, unsigned int height, unsigned int BPP, std::vector<unsigned char>& data)
{
    if (width * height * BPP != data.size())
    {
        LOG_ERROR("Texture data and specified parameters do not match!");
        return;
    }

    deleteTexture();

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
    if (BPP == 1)
    {
        // Set unpack alignment
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // Create ALPHA texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data.data());
    }
    else if (BPP == 3)
    {
        // Create RGB texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
    }
    else if (BPP == 4)
    {
        // Create RGBA texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
    }

    // Generate mip maps
    glGenerateMipmap(GL_TEXTURE_2D);
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

Audio::Audio()
{
    // Create source
    alGenSources((ALuint)1, &m_idSource);

    // Get error
    alGetError();
    ALenum alerror = alGetError();
    if (alerror != AL_NO_ERROR)
    {
        LOG_ERROR("OpenAL error '{0}', when creating a source!", alerror);
    }

    // Set default values
    setPitch(m_pitch);
    setVolume(m_volume);
    setPosition(m_position);
    setVelocity(m_velocity);
    setLooping(m_isLooping);
}

Audio::~Audio()
{
    // Free resources
    deleteAudio();
}

void Audio::start()
{
    if (m_idSource && m_idBuffer)
    {
        // Start playing the audio
        alSourcePlay(m_idSource);
    }
}

void Audio::pause()
{
    // Pause the audio
    alSourcePause(m_idSource);
}

void Audio::stop()
{
    // Stop the audio
    alSourceStop(m_idSource);
}

void Audio::setPitch(const float& value)
{
    // Set the pitch
    alSourcef(m_idSource, AL_PITCH, value);
    m_pitch = value;
}

const float& Audio::getPitch() const
{
    return m_pitch;
}

void Audio::setVolume(const float& value)
{
    // Set the volume
    alSourcef(m_idSource, AL_GAIN, value);
    m_volume = value;
}

const float& Audio::getVolume() const
{
    return m_volume;
}

void Audio::setPosition(const glm::vec3& value)
{
    // Set the position
    alSource3f(m_idSource, AL_POSITION, value.x, value.y, value.z);
    m_position = value;
}

const glm::vec3& Audio::getPosition() const
{
    return m_position;
}

void Audio::setVelocity(const glm::vec3& value)
{
    // Set the velocity
    alSource3f(m_idSource, AL_VELOCITY, value.x, value.y, value.z);
    m_velocity = value;
}

const glm::vec3& Audio::getVelocity() const
{
    return m_velocity;
}

void Audio::setLooping(const bool& value)
{
    // Set looping
    alSourcei(m_idSource, AL_LOOPING, (int)value);
    m_isLooping = value;
}

const bool& Audio::getLooping() const
{
    return m_isLooping;
}

void Audio::load()
{
    // Delete current texture if it exists
    deleteBuffer();

    // Load the texture
    loadBuffer();
}

void Audio::deleteAudio()
{
    // Free source
    alDeleteSources(1, &m_idSource);

    // Free buffer
    deleteBuffer();
}

void Audio::deleteBuffer()
{
    if (m_idBuffer)
    {
        // Free buffer
        alDeleteBuffers(1, &m_idBuffer);
    }
}

void Audio::loadBuffer()
{
    // Load audio
    Memory::reference<WaveFileContents> wave = readAudio(Source);

    if (!wave.valid())
    {
        LOG_WARN("Audio couldn't be created!");
        return;
    }

    // Generate buffer
    alGenBuffers((ALuint)1, &m_idBuffer);

    // Fill buffer data
    alBufferData(m_idBuffer,
        wave->Format,
        wave->Data.data(),
        wave->Header.DataSize,
        wave->Header.SampleRate);

    // Check for errors
    ALenum alerror = alGetError();
    if (alerror != AL_NO_ERROR)
    {
        LOG_ERROR("OpenAL error when filling buffer data!");
        return;
    }

    // Set source buffer
    alSourcei(m_idSource, AL_BUFFER, m_idBuffer);

    // Check for errors
    alerror = alGetError();
    if (alerror != AL_NO_ERROR)
    {
        LOG_ERROR("OpenAL error when setting source buffer!");
        return;
    }
}

UniformBuffer::UniformBuffer()
{
    glGenBuffers(1, &m_idUBO);
}

UniformBuffer::UniformBuffer(BoundPoints bp)
    : UniformBuffer()
{
    // Bind first
    bind();

    // Allocate
    allocBP(bp);
}

void UniformBuffer::bind()
{
    // Bind the buffer
    glBindBuffer(GL_UNIFORM_BUFFER, m_idUBO);
}

void UniformBuffer::allocBP(BoundPoints bp)
{
    // Get the size of the bound point
    m_size = getBPSize(bp);

    // Check if size is 0
    if (m_size == 0)
    {
        LOG_WARN("Allocating 0 size uniform buffer!");
    }

    // Allocate data
    glBufferData(GL_UNIFORM_BUFFER, m_size, NULL, GL_STATIC_DRAW);

    // Bind buffer range
    glBindBufferRange(GL_UNIFORM_BUFFER, bp, m_idUBO, 0, m_size);
}

void UniformBuffer::setSubData(const size_t& offset, const size_t& size, void* pValue)
{
    // Check if the buffer is large enough
    if (m_size < offset + size)
    {
        LOG_ERROR("UBO overflow caught, no updates applied!");
        return;
    }

    // Set sub data
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, pValue);
}

void UniformBuffer::mapData(void* pData)
{
    // Map buffer and get it's contents pointer
    void* ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);

    // Copy new data
    memcpy(ptr, pData, m_size);

    // Unmap the buffer
    glUnmapBuffer(GL_UNIFORM_BUFFER);
}

size_t UniformBuffer::getBPSize(BoundPoints bp)
{
    switch (bp)
    {
    case bp_Mat:
        return 2 * sizeof(glm::mat4);
    case bp_TexDetail:
        return 2 * sizeof(float);
    }

    return 0;
}

Text::Text()
    : m_pTexture(new Texture())
{
}

Text::~Text()
{
    for (auto& it : m_slots)
    {
        if (it.second.pVAO)
        {
            delete it.second.pVAO;
        }
    }
}

void Text::render()
{
    // Bind texture and shader
    m_pShader->bind();
    m_pTexture->bind();

    for (auto& it : m_slots)
    {
        Slot& slot = it.second;

        // Check if the slot is visible 
        if (slot.Visible)
        {
            // If needed regenerate the slot VAO
            if (slot.Regenerate)
            {
                // Update slot
                updateSlot(slot);
                slot.Regenerate = false;
            }
            
            // Bind and render
            slot.pVAO->bind();
            slot.pVAO->render();
        }
    }
}

void Text::load()
{
    // Load font
    Memory::reference<FontFileContents> font = readFont(FontSource);

    if (!font.valid())
    {
        LOG_WARN("Font couldn't be created!");
        return;
    }

    // Text BPP is 1
    unsigned int BPP = 1;

    // Create texture
    m_pTexture->load(font->Width, font->Height, BPP, font->Buffer);

    // Save metrics for later use
    m_metrics = font->Metrics;
    m_width = font->Width;
    m_height = font->Height;
}

void Text::setShader(Shader* pShader)
{
    m_pShader = pShader;
}

Text::Slot& Text::getSlot(const std::string& name)
{
    return m_slots[name];
}

void Text::updateSlot(Slot& slot)
{
    // Final buffers
    std::vector<float> vertices;
    std::vector<float> texCoord;

    // Reserve sizes
    vertices.reserve((slot.Content.length() * 6 * 3));
    texCoord.reserve((slot.Content.length() * 6 * 2));

    // The height of a single character this must match the one
    // that was used to load the font file
    unsigned int charHeight = 32;

    // Initialize the index to the vertex array.
    int index = 0;
    float scale = 1;

    // Copy since we use these variables for controlling the pen
    float x = slot.Position.x;
    float y = slot.Position.y;

    // Iterate over each text character
    std::string::const_iterator c;
    for (c = slot.Content.begin(); c != slot.Content.end(); c++)
    {
        // Calculate additional information needed to generate data
        CharMetric& metrics = m_metrics[*c];

        float xSize = metrics.End.x - metrics.Start.x;
        float ySize = metrics.End.y - metrics.Start.y;

        float xpos = x + metrics.Offset.x * scale;
        float ypos = y - (ySize - metrics.Offset.y) * scale;

        float w = xSize * scale;
        float h = ySize * scale;


        // First triangle
        vertices.push_back(xpos);
        vertices.push_back(ypos + h);
        vertices.push_back(1.0f);

        texCoord.push_back(metrics.Start.x / m_width);
        texCoord.push_back(metrics.Start.y / m_height);


        vertices.push_back(xpos + w);
        vertices.push_back(ypos);
        vertices.push_back(1.0f);

        texCoord.push_back(metrics.End.x / m_width);
        texCoord.push_back(metrics.End.y / m_height);


        vertices.push_back(xpos);
        vertices.push_back(ypos);
        vertices.push_back(1.0f);

        texCoord.push_back(metrics.Start.x / m_width);
        texCoord.push_back(metrics.End.y / m_height);


        // Second triangle in quad.
        vertices.push_back(xpos);
        vertices.push_back(ypos + h);
        vertices.push_back(1.0f);

        texCoord.push_back(metrics.Start.x / m_width);
        texCoord.push_back(metrics.Start.y / m_height);


        vertices.push_back(xpos + w);
        vertices.push_back(ypos + h);
        vertices.push_back(1.0f);

        texCoord.push_back(metrics.End.x / m_width);
        texCoord.push_back(metrics.Start.y / m_height);


        vertices.push_back(xpos + w);
        vertices.push_back(ypos);
        vertices.push_back(1.0f);

        texCoord.push_back(metrics.End.x / m_width);
        texCoord.push_back(metrics.End.y / m_height);

        x += metrics.Advance * scale;
    }

    // Update VAO or create and update if needed
    if (slot.pVAO == nullptr)
    {
        slot.pVAO = new VAO();
    }

    slot.pVAO->bind();
    slot.pVAO->createVerticesBuffer(vertices, true);
    slot.pVAO->createUVBuffer(texCoord, true);
}
