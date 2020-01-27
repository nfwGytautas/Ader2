#pragma once

// Name
#include <string>

// Forward declaration
struct Visual;

/**
 * GameObject represents a single object in the game
 */
struct GameObject
{
    /// Name of the object
    std::string Name;

    /// Current visual of the game object
    Memory::reference<Visual> CurrentVisual;
};
