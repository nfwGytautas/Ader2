#pragma once

// Forward declarations
class VAO;
class Shader;
class Texture;
class Audio;
class UniformBuffer;
struct AderScene;
struct GameObject;
struct AudioListener;

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// GLContext is part of the module system
#include "ModuleSystem/ModuleSystem.h"

// OpenGL includes
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// OpenAL includes
#include <AL/al.h>
#include <AL/alc.h>

// Window state structure
#include "Modules/InputInterface.h"

// Scenes
#include "MonoWrap/GLUE/AderScene.h"

// Graphics types are assets
#include "CommonTypes/Asset.h"

// Audio listener
#include "GameCore/AudioListener.h"


/**
 * Rendering settings containing, FoV, near and far plance
 */
struct RenderSettings
{
    /// Field of vision the renderer in degrees
    float FoV = 45.0f;

    /// Near rendering plane
    float NearPlane = 0.1f;

    /// Far rendering plane
    float FarPlane = 100.0f;
};


/**
 * This module provides a OpenGL based hardware acceleration for the engine
 *
 * MODULE
 * Messages:
 *  - StateBundleCreated
 *  - WndStateUpdated
 *  - WindowCreated
 *  - SystemUpdate
 *  - SystemRender
 *  - SceneChanged
 *
 * Posts:
 */
class GLContext : public Module
{
public:
    GLContext();

    // Inherited via Module
    virtual bool canShutdown() override;

    // Inherited via Module
    virtual void shutdown() override;

    // Inherited via Module
    virtual int onMessage(MessageBus::MessageType msg, MessageBus::DataType pData) override;

    // Toggle wire frame mode
    void toggleWireFrame(bool value);
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
     * Updates viewport and projection matrix when the window is updated
     */
    void wndResized();

    /**
     * Called when ever the render message is received
     *
     * @return 0 if there were no errors, otherwise error code
     */
    int render();

    /**
     * Change the current rendering scene to the one provided
     */
    void changeScene(MessageBus::DataType pData);

    /**
     * Update the context
     */
    void update();
private:
    /// Pointer to the window that the context is rendering to
    GLFWwindow* m_pRenderTarget;

    /// Pointer to OpenAL audio context
    ALCcontext* m_pAudioContext;

    /// Pointer to OpenAL audio device
    ALCdevice* m_pAudioDevice;

    /// Window state received from the InputInterface
    const WindowState* m_pWndState;

    /// Reference to the current active scene
    Memory::reference<AderScene> m_activeScene;

    /// Current projection matrix of the engine
    glm::mat4 m_projection;

    /// Current rendering settings of the engine
    RenderSettings m_settings;

    /// Matrices uniform buffer
    UniformBuffer* m_pubMatrices = nullptr;

    /// Texture detail uniform buffer
    UniformBuffer* m_pubTextureDetail = nullptr;

    /// Current audio listener
    AudioListener* m_pAudioListener = nullptr;
};


/**
 * Vertex array object class, this is used to specify the shape of the object
 * we want to render the other part is the textures
 */
class VAO : public Asset
{
private:
    enum AttribLocations
    {
        al_Vertices = 0,
        al_TexCoord = 1,

        // For storing transformation matrices during instance rendering
        al_Instance0 = 3,
        al_Instance1 = 4,
        al_Instance2 = 5,
        al_Instance3 = 6,

        // For storing the texture index in an atlas
        al_TexOffset = 7,
    };

    struct VBO
    {
        /// ID of the VBO
        unsigned int ID = 0;

        /// Allocated buffer size in bytes
        size_t Size = 0;

        /// Dynamic or not
        bool Dynamic = false;

        /// Type of the VBO
        int Type = -1;
    };
public:
    /**
     * Create empty VAO
     */
    VAO();

    ~VAO();

    /**
     * Render this VAO
     */
    void render();

    /**
     * Render this VAO count amount of times using instancing
     */
    void renderInstance(unsigned int count);

    /**
     * Create indices array for this array with the specified indices buffer
     *
     * @param indices Vector containing indices data
     * @param dynamic Boolean specifying if the indices buffer will be changed
     *                during runtime
     */
    void createIndiceBuffer(std::vector<unsigned int>& indices, bool dynamic);

    /**
     * Create vertices buffer for this VAO with the specified vertices buffer
     *
     * @param indices Vector containing vertices data
     * @param dynamic Boolean specifying if the vertices buffer will be changed
     *                during runtime
     */
    void createVerticesBuffer(std::vector<float>& vertices, bool dynamic);

    /**
     * Create texture coordinate buffer for this VAO with the specified coords buffer
     *
     * @param texCoords Vector containing texCoord data
     * @param dynamic Boolean specifying if the vertices buffer will be changed
     *                during runtime
     */
    void createUVBuffer(std::vector<float>& texCoords, bool dynamic);

    /**
     * Create transformation matrix instance buffer from the specified data
     *
     * @param transforms Vector containing transformation data
     * @param dynamic Boolean specifying if the vertices buffer will be changed
     *                during runtime
     */
    void createInstanceBuffer(std::vector<glm::mat4>& transforms, bool dynamic);

    /**
     * Create texture offset data buffer from the specified data
     *
     * @param offsets Vector containing offset data
     * @param dynamic Boolean specifying if the vertices buffer will be changed
     *                during runtime
     */
    void createOffsetBuffer(std::vector<glm::vec2>& offsets, bool dynamic);

    /**
     * Bind this VAO to the current OpenGL state machine.
     */
    void bind() const;

private:
    /**
     * Setup up the buffer and return if the buffer attributes need to
     * be setup
     *
     * @param vbo VBO to operate on
     * @param dynamic Is the buffer dynamic or not
     * @param eSize Size of a single element
     * @param eCount Count of elements
     * @param pData Data to of the buffer
     *
     * @return True if buffer attributes need to be setup, false otherwise
     */
    bool setupBuffer(VBO& buffer, bool dynamic, size_t eSize, size_t eCount, void* pData);

    /**
     * Deletes the specified buffer
     */
    void deleteBuffer(VBO& buffer);

    /**
     * Create buffer
     */
    void createBuffer(VBO& vbo);

    /**
     * Allocate a vertex buffer object of the specified size with specified init data and use
     *
     * @param vbo VBO to operate on
     * @param eSize Size of a single element
     * @param eCount Count of elements
     * @param pInitData Data to init the buffer with
     */
    void allocBuffer(VBO& vbo, size_t eSize, size_t eCount, void* pInitData);

    /**
     * Change buffer contents with the specified data
     *
     * @param vbo VBO to operate on
     * @param eSize Size of a single element
     * @param eCount Count of elements
     * @param pData Data to of the buffer
     */
    void modifyBuffer(VBO& vbo, size_t eSize, size_t eCount, void* pData);
private:
    unsigned int m_idArray = 0;

    VBO m_idIndices;
    VBO m_idVertices;
    VBO m_idTexCoords;
    VBO m_idInstance;
    VBO m_idOffsets;

    unsigned int m_renderCount = 0;
};


/**
 * Shader class is used to define how to interpret the vertex and texture data
 */
class Shader : public Asset
{
public:
    /// Source(path to the file) of the vertex shader
    std::string VertexSource;
    /// Source(path to the file) of the fragment shader
    std::string FragmentSource;

    /**
     * Create shader
     */
    Shader();

    ~Shader();

    /**
     * Bind this shader to the current OpenGL state machine.
     */
    void bind();

    /**
     * Load the shader with the specified paths
     */
    void load();
private:
    // Deletes the shader
    void deleteShader();

    // Loads the shader
    void loadShader();
private:
    unsigned int m_idShader = 0;
};


/**
 * Texture class, this is used to specify the texture of the object
 * we want to render the other part is the VAO
 */
class Texture : public Asset
{
public:
    /// Source(path to the file) of the texture
    std::string Source;

    /**
     * Create texture
     */
    Texture();

    ~Texture();

    /**
     * Bind this texture to the current OpenGL state machine 
     * at the specified texture slot.
     */
    void bind(unsigned int slot = 0);

    /**
     * Loads the texture from the source
     */
    void load();
private:
    // Deletes the texture
    void deleteTexture();

    // Loads the texture
    void loadTexture();
private:
    unsigned int m_idTexture = 0;
};


/**
 * Audio class used to provide sound to the game
 */
class Audio : public Asset
{
public:
    /// Source(path to the file) of audio source
    std::string Source;

    /**
     * Create Audio
     */
    Audio();

    ~Audio();

    /**
     * Start playing audio if it's loaded
     */
    void start();

    /**
     * Pause audio
     */
    void pause();

    /**
     * Stop playing audio
     */
    void stop();

    /**
     * Set the pitch value for the source
     *
     * @param value New pitch value
     */
    void setPitch(const float& value);

    /**
     * Get the pitch of the audio
     *
     * @return float representing audio pitch
     */
    const float& getPitch() const;

    /**
     * Set the volume value for the source
     *
     * @param value New volume value
     */
    void setVolume(const float& value);

    /**
     * Get the volume of the audio
     *
     * @return float representing audio volume
     */
    const float& getVolume() const;

    /**
     * Set the position value for the source
     *
     * @param value New position value
     */
    void setPosition(const glm::vec3& value);

    /**
     * Get the position of the audio
     *
     * @return glm::vec3 representing the audio position
     */
    const glm::vec3& getPosition() const;

    /**
     * Set the velocity value for the source
     *
     * @param value New velocity value
     */
    void setVelocity(const glm::vec3& value);

    /**
     * Get the velocity of the audio
     *
     * @return glm::vec3 representing the audio velocity
     */
    const glm::vec3& getVelocity() const;

    /**
     * Set if the source should loop
     *
     * @param value True the source will loop
     */
    void setLooping(const bool& value);

    /**
     * Check if the audio is looping
     *
     * @return True if the audio is currently in loop mode
     */
    const bool& getLooping() const;

    /**
     * Loads the audio from the source
     */
    void load();
private:
    // Deletes audio and buffer
    void deleteAudio();

    // Deletes audio buffer
    void deleteBuffer();

    // Loads audio buffer
    void loadBuffer();
private:
    unsigned int m_idSource = 0;
    unsigned int m_idBuffer = 0;

    float m_pitch = 1.0f;
    float m_volume = 1.0f;
    glm::vec3 m_position = glm::vec3(0, 0, 0);
    glm::vec3 m_velocity = glm::vec3(0, 0, 0);
    bool m_isLooping = false;
};


/**
 * Uniform buffer is used to provide uniform data to shaders
 */
class UniformBuffer
{
public:
    enum BoundPoints
    {
        bp_Mat = 0,
        bp_TexDetail = 1,
    };
public:
    /**
     * Creates empty uniform buffer
     */
    UniformBuffer();

    /**
     * Creates empty uniform buffer for the specified bound point
     */
    UniformBuffer(BoundPoints bp);

    /**
     * Binds this uniform buffer object, must be called before editing it's data
     */
    void bind();

    /**
     * Allocates memory for the uniform buffer with the size of the 
     * bounding point
     */
    void allocBP(BoundPoints bp);

    /**
     * Sets the sub data of the buffer from the offset to the specified value
     */
    void setSubData(const size_t& offset, const size_t& size, void* pValue);

    /**
     * Copies the specified data into the uniform buffer, this is the same as
     * calling setSubData with offset 0, size being the size of the entire data
     * structure and value being our data
     */
    void mapData(void* pData);

    /**
     * Returns the size in bytes of a buffer bound point
     */
    static size_t getBPSize(BoundPoints bp);
private:
    /// ID of the uniform buffer
    unsigned int m_idUBO = 0;

    /// Size allocated for this buffer
    size_t m_size = 0;
};
