#pragma once

// Forward declarations
class VAO;
class Shader;
class Texture;
struct AderScene;
struct GameObject;

// GLContext is part of the module system
#include "ModuleSystem/ModuleSystem.h"

// OpenGL includes
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Window state structure
#include "Modules/InputInterface.h"

// Scenes
#include "MonoWrap/GLUE/AderScene.h"

// Graphics types are assets
#include "CommonTypes/Asset.h"

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


/**
 * This module provides a OpenGL based hardware acceleration for the engine
 *
 * MODULE
 * Messages:
 *  - StateBundleCreated
 *  - WndStateUpdated
 *  - WindowCreated
 *  - SystemRender
 *  - SceneChanged
 *
 * Posts:
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
     * Called when ever the render message is received
     *
     * @return 0 if there were no errors, otherwise error code
     */
    int render();

    /**
     * Change the current rendering scene to the one provided
     */
    void changeScene(MessageBus::DataType pData);
private:
    /// Pointer to the window that the context is rendering to
    GLFWwindow* m_pRenderTarget;

    /// Window state received from the InputInterface
    const WindowState* m_pWndState;

    /// Reference to the current active scene
    Memory::reference<AderScene> m_activeScene;
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
