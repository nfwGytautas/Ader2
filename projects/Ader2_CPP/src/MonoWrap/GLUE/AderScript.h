#pragma once

#include "MonoWrap/MonoManager.h"

/**
 * C++ representation of the AderScript.cs
 */
struct AderScriptBase
{
    /// Class of ader script
    Memory::reference<SharpClass> Klass;

    /// Constructor of AderScript
    Memory::reference<SharpMethod> Constructor;

    /// Init method of AderScript
    Memory::reference<SharpMethod> Init;

    /// Update method of AderScript
    Memory::reference<SharpMethod> Update;
};


/**
 * C++ representation of any implementation of AderScript.cs
 */
class AderScript
{
public:
    /**
     * Create AderScript interface from the sharp class
     *
     * @param sClass SharpClass containing AderScript object
     */
    AderScript(AderScriptBase* base, Memory::reference<SharpClass> klass);

    /**
     * Get the name of the AderScript
     */
    const std::string& getName() const;

    /**
     * Invoke ctor for this object
     */
    void invokeConstruct();

    /**
     * Invoke Init for this object and return a sharp exception if
     * it occurred
     */
    Memory::reference<SharpException> invokeInit();

    /**
     * Invoke Update for this object and return a sharp exception if
     * it occurred
     */
    Memory::reference<SharpException> invokeUpdate();
private:
    /// Class representation of this implementation
    Memory::reference<SharpClass> m_class;

    /// Constructor of AderScript
    Memory::reference<SharpMethod> m_constructor;

    /// Init of AderScript
    Memory::reference<SharpMethod> m_init;

    /// Update of AderScript
    Memory::reference<SharpMethod> m_update;

    /// Instance of this implementation
    MonoObject* m_pInstance;

    /// Update thunk
    typedef void (*thunk_update)(MonoObject*, MonoObject**);
    thunk_update m_thunk_update;
};
