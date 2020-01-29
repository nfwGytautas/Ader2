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
    };
public:
    /**
     * Create empty VAO
     */
    VAO();

    ~VAO();

    void render();

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
     * Bind this VAO to the current OpenGL state machine.
     */
    void bind() const;

private:
    // Deletes the specified buffer
    void deleteBuffer(unsigned int& buffer);
private:
    unsigned int m_idArray = 0;

    unsigned int m_idIndices = 0;
    unsigned int m_idVertices = 0;
    unsigned int m_idTexCoords = 0;

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
