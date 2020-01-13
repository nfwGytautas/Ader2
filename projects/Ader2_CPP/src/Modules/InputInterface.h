#pragma once

// InputInterface is part of the module system
#include "ModuleSystem/ModuleSystem.h"

// States
#include "CommonTypes/States.h"

/**
 * InputInterface is used to provide a window a place to send inputs to
 * and at the same time provide all other modules access to input state.
 * This is done by listening to create window message, with the provided
 * window the InputInterface changes it's window procedure or sets callbacks.
 * After the callbacks the interface posts a message that states have been
 * created and that modules that require it should take a pointer to it.
 * During the update of the system if the state has been changed a message
 * sent for the state.
 *
 * MODULE
 * Messages:
 *  - WindowCreated
 *  - SystemUpdate
 *
 * Posts:
 *  - WndStateCreated
 *  - WndStateUpdate
 */
class InputInterface : public Module
{
public:
    InputInterface();

	// Inherited via Module
	virtual bool canShutdown() override;

	// Inherited via Module
	virtual void shutdown() override;

	// Inherited via Module
	virtual int onMessage(MessageBus::MessageType msg, MessageBus::DataType pData) override;

    /**
     * Query the current WindowState of the system
     *
     * @return const reference to the current WindowState
     */
    const WindowState& getWndState() const;

    /**
     * Query the current KeyboardState of the system
     *
     * @return const reference to the current KeyboardState
     */
    const KeyboardState& getKeyState() const;

private:
    /**
     * During this method the interface sends message input state pointers
     *
     * @return 0 if there were no errors, otherwise error code
     */
    int setup();

    /**
     * Initialize InputInterface with the specified window
     *
     * @param pWindow Pointer to the data passed in by the WindowCreated payload
     *
     * @return 0 if there were no errors, otherwise error code
     */
    int initWindowInput(MessageBus::DataType pWindow);

    /**
     * Update inputs, in this method the states are equalized so that in the case
     * that a module accidentally updates the states it would not cause problems
     * elsewhere
     *
     * @return 0 if there were no errors, otherwise error code
     */
    int update();

private:
    /**
     * Due to the nature of C APIs class methods cannot be passed as callbacks
     * because of this a static instance is provided, it's set during construction
     */
    static InputInterface* ms_pStaticThis;

    /**
     * Callback called when ever the window is resized
     *
     * @param width New width of the window
     * @param height New height of the window
     */
    void framebufferResize(int width, int height);

    /**
     * Callback called when ever the window is closed
     */
    void windowClose();

    /**
     * Callback called when ever a key state is changed
     *
     * @param key The key whose state changed
     * @param action The action that was taken: down, up, etc.
     * @param mods These are modification applied to the key: alt, shit, etc.
     */
    void keyInput(int key, int action, int mods);

private:
    // Actual state of inputs
    StateBundle m_actualState;

    // State of the ones sent through the message bus
    StateBundle m_msgState;

    /// Boolean to check if the window state changed since the last update
    bool m_wndStateChanged = false;

    /// Boolean to check if the keyboard state changed since the last update
    bool m_keyStateChanged = false;
};
