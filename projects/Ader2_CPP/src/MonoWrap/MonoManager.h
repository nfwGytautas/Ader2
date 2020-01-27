#pragma once

/**
 * @file
 *
 * Header containing the definition for the MonoManager class.
 * MonoManager is used to handle all C# related script files that means it loads assemblies,
 * creates images, can be used to find MonoClass definitions, instantiate these class and more.
 * 
 * The API is similar to Banshee3D engine MonoManager API
 */

// Used to specify paths and names for scripts
#include <string>
// Used to store class name and it's instance and SharpScript representation
#include <unordered_map>

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

// States
#include "CommonTypes/States.h"

// Forward declaration
class MonoManager;
class SharpDomain;
class SharpAssembly;
class SharpClass;
class SharpAttribute;
class SharpMethod;
class SharpProperty;
class SharpField;
class SharpException;
class SharpUtility;

// AderScript C++ interface
#include "GLUE/AderScript.h"

// AderEngine C++ interface
#include "GLUE/AderEngineSharp.h"

// AderScene C++ interface
#include "GLUE/AderScene.h"

struct AderScriptBase;
class AderScript;

struct AderEngineSharp;

struct AderSceneBase;
class AderScene;

/**
 * One of the main Ader modules.
 * This module manages all raw C# scripts and assemblies with a simpler interface.
 *
 * MODULE
 * Messages:
 *  - Setup
 *  - ScriptUpdate
 *  - LoadAssemblies
 *  - ReloadAssemblies
 *  - LoadScripts
 *  - ScriptUpdate
 *  - InitScripts
 *  - WndStateCreated
 *  - KeyStateCreated
 *  - LoadAderScenes
 *
 * Posts:
 *  - TransmitScenes
 */
class MonoManager : public Module
{
public:
    MonoManager();
    ~MonoManager();

    /**
     * Returns the current app domain
     */
    Memory::reference<SharpDomain> getDomain();

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
    virtual void shutdown() override;

    // Inherited via Module
    virtual int onMessage(MessageBus::MessageType msg, MessageBus::DataType pData) override;

private:
    /**
     * Setup the MonoManager and return the status
     *
     * @return 0 if there were no errors, otherwise error code
     */
    int setup();

    /**
     * Load all game assemblies in the specified directory
     *
     * @return 0 if there were no errors, otherwise error code
     */
    int loadAssemblies(const std::string& assemblyDirectory);

    /**
     * Reload all loaded game assemblies
     *
     * @return 0 if there were no errors, otherwise error code
     */
    int reloadAssemblies();

    /**
     * Find and create all objects that inherit Script object
     *
     * @return 0 if there were no errors, otherwise error code
     */
    int loadScripts();

    /**
     * Initializes all script objects
     *
     * @return 0 if there were no errors, otherwise error code
     */
    int initScripts();

    /**
     * Updates all script objects
     *
     * @return 0 if there were no errors, otherwise error code
     */
    int updateScripts();

    /**
     * Sets the state bundle
     *
     * @param pState The state to set as
     *
     * @return 0 if there were no errors, otherwise error code
     */
    int setStateBundle(void* pState);

    /**
     * Initializes the Ader2_SHARP engine
     */
    void initEngine();

    /**
     * Sets AderEngine fields
     * -WindowState -> _wndState
     * -KeyboardState -> _keyState
     */
    void setFields();

    /**
     * Find and create all objects that inherit Scene object
     *
     * @return 0 if there were no errors, otherwise error code
     */
    int loadAderScenes();

    /**
     * Links C++ functions to C# methods
     */
    void addInternalCalls();
private:
    /**
     * Due to the nature of C APIs class methods cannot be passed as callbacks
     * because of this a static instance is provided, it's set during construction
     */
    static MonoManager* ms_pStaticThis;

    /// State bundle received from the message bus
    StateBundle* m_pStates;

    /// Current app domain iteration
    int m_appDomainIter = 0;

    /// The main MonoDomain
    MonoDomain* m_pMainDomain = nullptr;

    /// The current app domain
    Memory::reference<SharpDomain> m_appDomain = nullptr;

    /// Reference to Ader2_SHARP assembly
    Memory::relay_ptr<SharpAssembly> m_engineAssembly;

    /// Pointer to ader script base interface
    AderScriptBase* m_pAderScriptBase = nullptr;

    /// Pointer to ader scene base interface
    AderSceneBase* m_pAderSceneBase = nullptr;

    /// Pointer to ader engine interface
    AderEngineSharp* m_pAderEngine = nullptr;

    /// Vector of all loaded scripts
    std::vector<Memory::reference<AderScript>> m_scripts;

    /// Vector of all loaded scenes
    std::vector<Memory::reference<AderScene>> m_scenes;

    /// Vector containing all requests to load assemblies
    std::vector<std::string> m_loadDirs;
};


/**
 * Wrapper around MonoDomain
 * This wrapper provides:
 *  -A way to create a new domain
 *  -A way to unload a domain
 *  -A way to load an assembly
 *  -A way to get all classes that inherit a specified parent
 *  -A way to get a class from all loaded assemblies
 *  -A way to create a string in this current domain
 */
class SharpDomain
{
public:
    /**
     * Creates a new domain with the specified name
     */
    SharpDomain(const std::string& name);

    ~SharpDomain();

    /**
     * Loads a C# assembly from the specified path and returns a reference to it.
     *
     * @param folder Folder of the assembly
     * @param name Name of the assembly
     * @return Valid Memory::reference<SharpAssembly> if loaded successfully,
     *         Invalid otherwise
     */
    Memory::relay_ptr<SharpAssembly> loadAssembly(const std::string& folder, const std::string& name);

    /**
     * Tries to find the specified assembly. An invalid reference is returned if it
     * can't be found.
     *
     * @param name The name of the assembly
     *
     * @return Valid Memory::reference<SharpAssembly> if assembly has been found and loaded, an
     *         invalid one if it failed
     */
    Memory::relay_ptr<SharpAssembly> getAssembly(const std::string& name);

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
     * Finds and returns all classes that inherit from the specified base class, not including
     * itself if it finds it
     *
     * @param base Reference to the base class
     *
     * @return std::vector of valid Memory::reference<SharpClass> objects
     */
    std::vector<Memory::reference<SharpClass>> getClassInherits(Memory::reference<SharpClass> base);

    /**
     * Creates a string from the passed text in the current domain
     *
     * @param from String to convert
     *
     * @return MonoString pointer
     */
    MonoString* createString(const std::string& from);

    /**
     * Unloads the domain and closes all of its assemblies
     */
    void unload();

private:
    /// Domain pointer
    MonoDomain* m_pDomain;

    /// Flag to check if the domain was unloaded already
    bool m_unloaded = false;

    /// Name of the domain
    std::string m_name;

    /// Vector of all loaded assemblies in this domain
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

    ~SharpAssembly();

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
    /// Pointer to the domain that this assembly belongs to
    MonoDomain* m_pDomain = nullptr;
    /// Pointer to the assembly
    MonoAssembly* m_pAssembly = nullptr;
    /// Pointer to the image
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
 *  -A way to get base class
 *  -A way to check if the class has a parent class
 *  -A way to check if a specific class is base of this class
 *  -A way to create a field from this class
 *  -A way to get a property from this class
 *  -A way to get all custom attributes of the class
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
     * Create SharpClass from specified MonoClass
     *
     * @param pDomain MonoDomain pointer
     * @param pImage MonoImage pointer
     * @param pClass MonoClass pointer
     */
    SharpClass(MonoDomain* pDomain, MonoImage* pImage, MonoClass* pClass);

    /**
     * Get the name of this class
     */
    const std::string& getName();

    /**
     * Get the namespace of this class
     */
    const std::string& getNamespace();

    /**
     * True if the class inherits any other object
     */
    bool hasBaseClass();

    /**
     * True if the specified class is base of this class, false otherwise
     *
     * @param base SharpClass to the class you want to test for
     */
    bool hasBaseClass(Memory::reference<SharpClass> base);

    /**
     * Get the base class of this class object
     *
     * @return Memory::reference<SharpClass> for the base class
     */
    Memory::reference<SharpClass> getBaseClass();

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
     * Returns a reference to the SharpProperty with the specified name and parameter combination.
     *
     * @param name name of the property
     *
     * @return Valid Memory::reference<SharpProperty> if the property was found,
     *         invalid otherwise
     */
    Memory::reference<SharpProperty> getProperty(const std::string& name);

    /**
     * Returns references of the class properties
     *
     * @return Vector containing references to all class properties
     */
    std::vector<Memory::reference<SharpProperty>> getAllProperties();

    /**
     * Returns a reference to the SharpField with the specified name and parameter combination.
     * This is the main way to create a SharpField and should be preferred.
     *
     * @param name name of the property
     *
     * @return Valid Memory::reference<SharpField> if the field was found,
     *         invalid otherwise
     */
    Memory::reference<SharpField> getField(const std::string& name);

    /**
     * Gets all attributes of this class
     *
     * @return Vector containing all attributes
     */
    std::vector<Memory::reference<SharpAttribute>> getAttributes();

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
    /// Pointer to the domain that this assembly belongs to
    MonoDomain* m_pDomain = nullptr;
    /// Pointer to the image
    MonoImage* m_pImage = nullptr;
    /// Pointer to the class definition
    MonoClass* m_pClass = nullptr;

    /// Name of the class
    std::string m_name = "";
    /// Namespace of the class
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
     * @param pInstance Object instance
     * @param params Parameters
     * @param exception Optional exception parameter
     *
     * @return Result of the method
     */
    MonoObject* invokeMethod(MonoObject* pInstance, void** params, MonoObject** exception = nullptr);

    /**
     * Invokes a virtual method for the instance with the specified parameters
     *
     * @param pInstance Object instance
     * @param params Parameters
     * @param exception Optional exception parameter
     */
    MonoObject* invokeVirtualMethod(MonoObject* pInstance, void** params, MonoObject** exception = nullptr);

    /**
     * Creates a virtual SharpMethod from this method
     *
     * @param pInstance Object instance
     *
     * @return Memory::reference<SharpMethod> to the new virtual method
     */
    Memory::reference<SharpMethod> getAsVirtual(MonoObject* pInstance);

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
    /// Pointer to the instance of the method
    MonoMethod* m_pMethod = nullptr;

    /// Name of the method
    std::string m_fullName = "";
};


/**
 * Wrapper around a MonoProperty
 * This wrapper provides:
 *  -A way to get a class property
 *  -A way to set a class property
 */
class SharpProperty
{
public:
    /**
     * Creates SharpProperty instance from MonoProperty
     *
     * @param pProperty MonoProperty pointer
     */
    SharpProperty(MonoProperty* pProperty);

    /**
     * Creates SharpProperty instance from MonoClass pointer and name
     *
     * @param pClass MonoClass pointer
     * @param name Name of the property
     */
    SharpProperty(MonoClass* pClass, const std::string& name);

    /**
     * Get the name of this property
     */
    const std::string& getName();

    /**
     * Get the value of the property for the specified object
     *
     * @param pInstance MonoObject pointer
     *
     * @return Pointer to a MonoObject use converter
     * methods to get the value
     */
    MonoObject* getValue(MonoObject* pInstance);

    /**
     * Set the value of the property for the specified object
     *
     * @param pInstance MonoObject pointer
     * @param params Pointer to the array value that contains 1 value
     */
    void setValue(MonoObject* pInstance, void** params);

    /**
     * Return the thunk for the property get method
     */
    void* getGetThunk();

    /**
     * Return the thunk for the property set method
     */
    void* getSetThunk();

    /**
     * Returns true if the property was loaded correctly
     */
    bool loaded();
private:
    /// Pointer to the instance of the property
    MonoProperty* m_pProperty = nullptr;

    /// Get method
    Memory::reference<SharpMethod> m_getMethod;

    /// Set method
    Memory::reference<SharpMethod> m_setMethod;
};


/**
 * Wrapper around mono class attributes
 * This wrapper provides:
 *  -A way to get name of the attribute
 *  -A way to get data of the attribute
 *  -A way to get the class of the attribute
 */
class SharpAttribute
{
public:
    /**
     * Create an attribute from the specified class and object
     *
     * @param klass Reference to the attribute class
     * @param pObject Pointer to the attribute instance
     */
    SharpAttribute(Memory::reference<SharpClass> klass, MonoObject* pObject);

    /**
     * Get the value of the specified attribute property
     *
     * @param prop Name of the property
     *
     * @return The property value, use SharpMarshal to get C++ type
     */
    MonoObject* getValue(const std::string& prop);

    /**
     * Returns the reference to the sharp class of the attribute
     */
    Memory::reference<SharpClass> getClass();
private:
    /// Reference of the attribute class
    Memory::reference<SharpClass> m_class;

    /// Instance of the attribute
    MonoObject* m_pInstance;

    /// Attribute property map
    std::unordered_map<
        std::string, 
        Memory::reference<SharpProperty>> m_properties;
};


/**
 * Wrapper around MonoClassField
 * This wrapper provides:
 *  -A way to get a class field
 *  -A way to set a class field
 *  -Check if the field is static
 */
class SharpField
{
public:
    /**
     * Creates SharpField instance from MonoClassField
     *
     * @param pDomain MonoDomain pointer
     * @param pField MonoClassField pointer
     */
    SharpField(MonoDomain* pDomain, MonoClassField* pField);

    /**
     * Get the value of the field for the specified object nullptr for
     * static field
     *
     * @param pInstance MonoObject pointer nullptr for static
     * @param value Pointer to where the result will be stored
     */
    void getValue(MonoObject* pInstance, void* value);

    /**
     * Set the value of the property for the specified object nullptr for
     * static field
     *
     * @param pInstance MonoObject pointer null for static fields
     * @param value Pointer to the value
     */
    void setValue(MonoObject* pInstance, void* value);

    /**
     * Returns true if the field is static, false otherwise
     */
    bool isStatic() const;

    /**
     * Returns true if the field was loaded correctly
     */
    bool loaded();
private:
    /// Pointer to the instance of the field
    MonoClassField* m_pField = nullptr;

    /// Pointer to the field class virtual table
    MonoVTable* m_pVTable = nullptr;

    /// Boolean to check if the field is static or not
    bool m_isStatic = false;

    /// Name of the property
    std::string m_fullName = "";
};


/**
 * Wrapper around MonoException/MonoObject
 * This wrapper provides:
 *  -A way to check the stack trace of the exception
 *  -A way to check the message of the exception
 */
class SharpException
{
public:
    /**
     * Creates SharpException instance from MonoObject
     *
     * @param pException MonoObject pointer
     */
    SharpException(MonoObject* pException);

    /**
     * Returns the message of the exception
     */
    const std::string& getMessage() const;

    /**
     * Returns the stack trace of the exception
     */
    const std::string& getStackTrace() const;

private:
    /**
     * Gets m_msg and m_stackTrace values
     *
     * @param pException Pointer to the exception
     */
    void getValues(MonoObject* pException);
private:
    /// Message of the exception
    std::string m_msg;

    /// Stack trace of the exception
    std::string m_stackTrace;
};


/**
 * This class provides ways to marshal MonoObject pointers
 * that are retrieved from invoking SharpMethods and make
 * them into C++ types and vice versa.
 * Also other helper methods.
 */
class SharpUtility
{
public:
    /**
     * Convert the specified MonoObject into a string and return it
     */
    static std::string toString(MonoObject* pObject);

    /**
     * Create a method signature from namespace, class, method, and signature
     *
     * @param nSpace Namespace of the class method separated in dots e.g. nSpace1.nSpace2
     * @param klass Class of the method
     * @param method Name of the method
     * @param params Method parameters in parentheses e.g. (int, double)
     * @param isStatic Is the method static or not, default false
     */
    static std::string methodSignature(const std::string& nSpace,
                                       const std::string& klass,
                                       const std::string& method,
                                       const std::string& params,
                                       bool isStatic = false);
};
