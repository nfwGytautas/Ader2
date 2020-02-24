#pragma once

// GLM
#include <glm/glm.hpp>

/**
 * This struct is used to define properties for the audio listener
 * of the engine
 */
struct AudioListener
{
    /**
     * If the audio listener is altered then the context 
     * updates values and resets this value. This value
     * is automatically set by the engine when necessary
     */
    bool Altered = true;

    /// Position of the listener
    glm::vec3 Position;

    /// Global volume
    float Volume = 0.5f;

    struct _Orientation
    {
        /**
         * First half of the listener orientation matrix.
         * Default 0, 0, -1 the at orientation for the listener.
         * This is similar to the target vector of the camera
         */
        glm::vec3 At = glm::vec3(0.0f, 0.0f, -1.0f);

        /**
         * Second half of the listener orientation matrix.
         * Default 0, 1, 0 the up orientation for the listener.
         * This is similar to the up vector of the camera
         */
        glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
    } Orientation;
};
