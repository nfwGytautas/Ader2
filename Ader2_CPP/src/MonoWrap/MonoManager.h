#pragma once

/**
 * Header containing the definition for the MonoManager class.
 * MonoManager is used to handle all C# related script files that means it loads assemblies,
 * creates images, can be used to find MonoClass definitions, instantiate these class and more.
 * 
 * The API is similar to Banshee3D engine MonoManager API
 */

// Used to specify paths and names for scripts
#include <string>

// Mono dependencies
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-config.h>

#ifdef ADER_MACRO_DEBUG
#include <mono/metadata/debug-helpers.h>
#endif


// MonoManager is a module of the AderEngine
#include "ModuleSystem/ModuleSystem.h"

// Memory types
#include "CommonTypes/reference.h"


// Forward declaration
class MonoManager;
class SharpAssembly;
class SharpClass;
class SharpMethod;


/**
 * One of the main Ader modules.
 * This module manages all raw C# scripts and assemblies with a simpler interface.
 */
class MonoManager : public Module
{
public:
    /**
     * Loads a C# assembly from the specified path and returns a reference to it.
     *
     * @param folder Folder of the assembly
     * @param name Name of the assembly
     * @return Valid Memory::reference<SharpAssembly> if loaded successfully,
     *         Invalid otherwise
     */
    Memory::reference<SharpAssembly> loadAssembly(const std::string& folder, const std::string& name);

    /**
     * Tries to find the specified assembly. An invalid reference is returned if it
     * can't be found.
     *
     * @param name The name of the assembly
     *
     * @return Valid Memory::reference<SharpAssembly> if assembly has been found and loaded, an
     *         invalid one if it failed
     */
    Memory::reference<SharpAssembly> getAssembly(const std::string& name);

    /**
     * Tries to find the specified class. An invalid reference is returned if it
     * can't be found.
     *
     * @param nSpace The namespace of the class
     * @param name The name of the class
     * 
     * @return Valid Memory::reference<SharpClass> if class has been found, an
     *         invalid one if it failed
     */
    Memory::reference<SharpClass> getClass(const std::string& nSpace, const std::string& name);

    /**
     * Reloads the domain, and clears all managed objects.
     */
    void reload();

    /**
     * Creates a string from the passed text in the current domain
     *
     * @param from String to convert
     *
     * @return MonoString pointer
     */
    MonoString* createString(const std::string& from);

    /**
     * Maps the managed method with provided name with the specified callback
     *
     * @param name Full name of the method/signature
     * @param callback Callback to map to
     */
    void addInternalCall(const std::string& name, const void* callback);

    // Inherited via Module
    virtual bool canShutdown() override;

    // Inherited via Module
    virtual int onMessage(MessageBus::MessageType msg, MessageBus::DataType pData) override;

private:
    /**
     * Setup the MonoManager and return the status
     *
     * @return 0 if there were no errors, otherwise error code
     */
    int setup();

private:
    // The current MonoDomain
    MonoDomain* m_pDomain = nullptr;

    // Vector of all loaded assemblies
    std::vector<Memory::reference<SharpAssembly>> m_assemblies;
};


/**
 * Wrapper around MonoAssembly.
 * This wrapper provides:
 *  -A way to get all classes that exist in the assembly.
 *  -A way to get a specific class by its name
 *  -A way to get the name of the assembly
 */
class SharpAssembly
{
public:
    /**
     * Loads assembly into the specified domain from the specified path
     *
     * @param pDomain MonoDomain pointer
     * @param folder Folder where the assembly is located in
     * @param name Name of the assembly
     */
    SharpAssembly(MonoDomain* pDomain, const std::string& folder, const std::string& name);

    /**
     * Returns a class that is from the specified namespace and with the specified name
     * 
     * @param nSpace Namespace for the class
     * @param name Name of the class
     *
     * @return Valid Memory::reference<SharpClass> if the class has been found,
     *         invalid otherwise
     */
    Memory::reference<SharpClass> getClass(const std::string& nSpace, const std::string& name) const;

    /**
     * Returns all classes in this assembly
     *
     * @return A vector of containing valid Memory::reference<SharpClass> if the class has been found,
     *         invalid otherwise
     */
    std::vector<Memory::reference<SharpClass>> getAllClasses() const;

    /**
     * Returns the name of the assembly
     *
     * @return std::string representation of the name
     */
    const std::string& getName() const;

    /**
     * Returns true if the assembly was loaded correctly
     */
    bool loaded();
private:
    // Pointer to the domain that this assembly belongs to
    MonoDomain* m_pDomain = nullptr;
    // Pointer to the assembly
    MonoAssembly* m_pAssembly = nullptr;
    // Pointer to the image
    MonoImage* m_pImage = nullptr;

    /// Name of the assembly
    std::string m_name = "";
};


/**
 * Wrapper around MonoClass
 * This wrapper provides:
 *  -A way to invoke a static class method
 *  -A way to add internal calls for this specific class
 *  -A way to get all methods of the class
 *  -A way to get a single method of the class
 *  -A way to instantiate a class of this type
 */
class SharpClass
{
public:
    /**
     * Try and create nSpace.name MonoClass from the specified assembly
     *
     * @param pDomain MonoDomain pointer
     * @param pImage MonoImage pointer
     * @param nSpace Namespace of the class
     * @param name Name of the class
     */
    SharpClass(MonoDomain* pDomain, MonoImage* pImage, const std::string& nSpace, const std::string& name);

    /**
     * Get the name of this class
     */
    const std::string& getName();

    /**
     * Get the namespace of this class
     */
    const std::string& getNamespace();

    /**
     * Maps this class method with provided name with the specified callback
     *
     * @param name Method name to map
     * @param callback Callback to map to
     */
    void addInternalCall(const std::string& name, const void* callback);

    /**
     * Invokes a static method with the specified name and params
     *
     * @param name Name of the static method
     * @param signatureParams Parameters for the signature
     * @param params Parameters
     */
    MonoObject* invokeMethod(const std::string& name, const std::string& signatureParams, void** params);

    /**
     * Returns all the methods that this class has
     *
     * @return std::vector<Memory::reference<SharpMethod>> of all methods
     */
    std::vector<Memory::reference<SharpMethod>> getAllMethods();

    /**
     * Returns a reference to the SharpMethod with the specified name and parameter combination.
     * For example to call a constructor of 2 strings it would be 'ctor' for the name and
     * 'string,string' for the parameters.
     *
     * @param name name of the method
     * @param params Parameters of the method
     * @param isStatic True if the method is static
     *
     * @return Valid Memory::reference<SharpMethod> if the method was found,
     *         invalid otherwise
     */
    Memory::reference<SharpMethod> getMethod(const std::string& name, const std::string& params, bool isStatic);

    /**
     * Creates a MonoObject pointer from this class
     */
    MonoObject* createInstance();

    /**
     * Returns true if the class was loaded correctly
     */
    bool loaded();
private:
    /**
     * Create the signature of the method for this class
     *
     * @param name Name of the method
     * @param params Parameters of the method
     * @param isStatic True if the method is static, False otherwise
     *
     * @return Signature, Namespace.Name:Method(params) for static methods
     *                    Namespace.Name::Method(params) for non static methods
     */
    std::string methodSignature(const std::string& name, const std::string& params = "", bool isStatic = false);
private:
    // Pointer to the domain that this assembly belongs to
    MonoDomain* m_pDomain = nullptr;
    // Pointer to the image
    MonoImage* m_pImage = nullptr;
    // Pointer to the class definition
    MonoClass* m_pClass = nullptr;

    // Name of the class
    std::string m_name = "";
    // Namespace of the class
    std::string m_nSpace = "";
};


/**
 * Wrapper around a MonoMethod
 * This wrapper provides:
 *  -A way to invoke a class method
 *  -A way to invoke a virtual method
 *  -A way to create a method thunk
 *  -A way to get method full name
 */
class SharpMethod
{
public:
    /**
     * Creates SharpMethod instance from MonoMethod
     *
     * @param pMethod MonoMethod pointer
     */
    SharpMethod(MonoMethod* pMethod);

    /**
     * Creates SharpMethod instance from MonoImage pointer
     *
     * @param pImage MonoImage pointer
     * @param signature Signature of the method
     */
    SharpMethod(MonoImage* pImage, const std::string& signature);

    /**
     * Invokes a method for the instance with the specified parameters
     *
     * @param instance Object instance
     * @param params Parameters
     *
     * @return Result of the method
     */
    MonoObject* invokeMethod(MonoObject* instance, void** params);

    /**
     * Invokes a virtual method for the instance with the specified parameters
     *
     * @param instance Object instance
     * @param params Parameters
     */
    MonoObject* invokeVirtualMethod(MonoObject* instance, void** params);

    /**
     * Returns a thunk of this method
     *
     * @return A function thunk, for more check out Mono documentation
     */
    void* getThunk();

    /**
     * Returns the full name of the method
     */
    const std::string& getFullName();

    /**
     * Returns true if the method was loaded correctly
     */
    bool loaded();
private:
    // Pointer to the instance of the class
    MonoMethod* m_pMethod = nullptr;

    // Name of the method
    std::string m_fullName = "";
};
