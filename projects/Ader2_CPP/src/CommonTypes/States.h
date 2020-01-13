#pragma once

// For the Key enum
#include "Enums/Input.h"

/**
 * Struct that contains the information of the current window state.
 * Can be queried from the InputInterface
 */
struct WindowState
{
    /// Current width of the window
    int width;

    /// Current height of the window
    int height;

    /// True if the window is closed and false if it is opened
    bool closed = false;

    /// True if the window was resized in the update
    bool resized = false;
};

/**
 * Struct that contains the information of the current keyboard state.
 * Can be queried from the InputInterface
 */
struct KeyboardState
{
    /**
     * Keyboard key array that specified if a key is either down or up
     * to check specific one use Key enum
     */
    bool Keys[InputEnums::KEY_ENUM_SIZE] = {false};
};

/**
 * Struct containing all states for easier messaging
 */
struct StateBundle
{
    WindowState WndState;
    KeyboardState KeyState;
};