#include "MonoManager.h"

// For find_if
#include <algorithm>

// For browsing directories
#include <filesystem>

// Engine messages
#include "Enums/Messages.h"

// Logger
#include "Utility/Log.h"

// Assert
#include "Defs.h"

// Field attributes
#include <mono/metadata/attrdefs.h>
#include <mono/metadata/mono-gc.h>
#include <mono/metadata/threads.h>

#ifdef ADER_MACRO_DEBUG
#include <mono/utils/mono-logger.h>
#include <mono/metadata/mono-debug.h>
#endif

// Definitions
#define ASSEMBLY_NAME "Ader2_SHARP.dll"
#define SCRIPT_NAMESPACE "Ader2"
#define ENGINE_NAMESPACE "Ader2.Core"
#define SCRIPT_CLASS_NAME "AderScript"
#define ENGINE_CLASS_NAME "AderEngine"
#define SCRIPT_CLASS_FILTER(x) (x == "Internal" || x == "<Module>")

#include <iostream>

MonoManager* MonoManager::ms_pStaticThis = nullptr;

MonoManager::MonoManager()
	: m_pAderScriptBase(new AderScriptBase()), m_pAderEngine(new AderEngineSharp())
{
	ms_pStaticThis = this;
}

MonoManager::~MonoManager()
{
	if (m_pAderScriptBase)
	{
		delete m_pAderScriptBase;
	}
	
	if (m_pAderEngine)
	{
		delete m_pAderEngine;
	}
}

Memory::reference<SharpDomain> MonoManager::getDomain()
{
	return m_appDomain;
}

void MonoManager::addInternalCall(const std::string& name, const void* callback)
{
	// Add a mono internal call
	mono_add_internal_call(name.c_str(), callback);
}

bool MonoManager::isClosed() const
{
	return m_closed;
}

bool MonoManager::canShutdown()
{
	// Currently not blocking
	return true;
}

void MonoManager::shutdown()
{
	// Destroy the current domain first
	m_appDomain = nullptr;

	// Clean up the domain
	mono_jit_cleanup(m_pMainDomain);
}

int MonoManager::onMessage(MessageBus::MessageType msg, MessageBus::DataType pData)
{
	switch (msg)
	{
	case Messages::msg_Setup:
		return setup();
	case Messages::msg_LoadAssemblies:
		return loadAssemblies(*static_cast<const std::string*>(pData));
	case Messages::msg_CloseAssemblies:
		return closeAssemblies();
	case Messages::msg_ReloadAssemblies:
		return reloadAssemblies();
	case Messages::msg_LoadScripts:
		return loadScripts();
	case Messages::msg_InitScripts:
		return initScripts();
	case Messages::msg_ScriptUpdate:
		return updateScripts();
	case Messages::msg_StateBundleCreated:
		return setStateBundle(pData);
	}

	return 0;
}

int MonoManager::setup()
{
	// Set directories where mono libraries are at
	// By default using premake they will be copied to 
	// lib and etc folders in the output folder
	mono_set_dirs("mono/lib", "mono/etc");

	
	// Load the default Mono configuration file, this is needed
	// if you are planning on using the dll maps defined on the
	// system configuration
	mono_config_parse(NULL);

#ifdef ADER_MACRO_DEBUG
	// clang-format off
	const char* options[] =
	{
		"--soft-breakpoints",
		"--debugger-agent=transport=dt_socket,suspend=n,server=y,address=127.0.0.1:55555,embedding=1",
		"--debug-domain-unload",

		// GC options:
		// check-remset-consistency: Makes sure that write barriers are properly issued in native code,
		// and therefore
		//    all old->new generation references are properly present in the remset. This is easy to mess
		//    up in native code by performing a simple memory copy without a barrier, so it's important to
		//    keep the option on.
		// verify-before-collections: Unusure what exactly it does, but it sounds like it could help track
		// down
		//    things like accessing released/moved objects, or attempting to release handles for an
		//    unloaded domain.
		// xdomain-checks: Makes sure that no references are left when a domain is unloaded.
		"--gc-debug=check-remset-consistency,verify-before-collections,xdomain-checks"
	};
	// clang-format on
	mono_jit_parse_options(sizeof(options) / sizeof(char*), const_cast<char**>(options));
	mono_debug_init(MONO_DEBUG_FORMAT_MONO);
#endif

	// Create the mono domain
	m_pMainDomain = mono_jit_init_version("MonoManager", "v4.0.30319");

	// Initialize engine
	initEngine();

	return 0;
}

int MonoManager::loadAssemblies(const std::string& assemblyDirectory)
{
	// Check if haven't loaded this directory already
	if (std::find(m_loadDirs.begin(), m_loadDirs.end(), assemblyDirectory) == m_loadDirs.end())
	{
		m_loadDirs.push_back(assemblyDirectory);
	}

	// Iterate over all files inside the directory and then load them
	for (const auto& entry : std::filesystem::directory_iterator(assemblyDirectory))
	{
		// Make sure we are looking at a file
		if (!entry.is_directory())
		{
			auto& path = entry.path();

			// Check for .dll extension
			if(path.has_extension() && path.extension() == ".dll")
			{
				std::string& file = path.filename().string();
				LOG_DEBUG("Loading assembly '{0}/{1}'", assemblyDirectory, file);
				m_appDomain->loadAssembly(assemblyDirectory, file);
			}
		}
	}

	return 0;
}

int MonoManager::closeAssemblies()
{
	if (m_closed)
	{
		LOG_WARN("Trying to close an already closed manager!");
		return 1;
	}

	m_engineAssembly = nullptr;

	// Clear all scripts
	m_scripts.clear();

	// Unload the domain
	m_appDomain->unload();

	// Set closed flag
	m_closed = true;

	return 0;
}

int MonoManager::reloadAssemblies()
{
	if (!m_closed)
	{
		LOG_WARN("Trying to reload assemblies before closing them!");
		return 1;
	}

	// Initialize engine
	initEngine();

	// Set states
	if (m_pStates)
	{
		// Set fields
		setFields();
	}

	int result = 0;

	// Iterate over all load directories
	for (const std::string& dir : m_loadDirs)
	{
		result = loadAssemblies(dir);

		if (result != 0)
		{
			LOG_ERROR("reloadAssemblies.loadAssemblies error '{0}' with '{1}'", result, dir);
			result = 0;
		}
	}

	// Load all scripts
	result = loadScripts();

	// Invoke init on all scripts
	result = initScripts();

	return result;
}

int MonoManager::loadScripts()
{
	// Iterate over all assembly classes that inherit AderScript
	for (const Memory::reference<SharpClass>& klass : m_appDomain->getClassInherits(m_pAderScriptBase->Klass))
	{
		// Push back the new script
		m_scripts.push_back(new AderScript(m_pAderScriptBase, klass));
		LOG_DEBUG("Loaded '{0}' script", klass->getName());
	}

	// Set closed flag
	m_closed = false;

	// Construct all classes
	for (const Memory::reference<AderScript>& script : m_scripts)
	{
		script->invokeConstruct();
	}

	return 0;
}

int MonoManager::initScripts()
{
	// Return if the manager is closed
	if (m_closed)
	{
		LOG_WARN("Trying to call script objects on closed manager!");
		return 1;
	}

	// Invoke all updates
	for (const Memory::reference<AderScript>& script : m_scripts)
	{
		// Invoke Update
		Memory::reference<SharpException> ex = script->invokeInit();

		// Check if there was an exception
		if (ex.valid())
		{
			LOG_WARN("Script '{0}' raised an exception!\n Message: '{1}'\n Stack trace:\n{2}",
				script->getName(), ex->getMessage(), ex->getStackTrace());
		}
	}

	return 0;
}

int MonoManager::updateScripts()
{
	// Return if the manager is closed
	if (m_closed)
	{
		LOG_WARN("Trying to call script objects on closed manager!");
		return 1;
	}

	// Invoke all updates
	for (const Memory::reference<AderScript>& script : m_scripts)
	{
		// Invoke Update
		Memory::reference<SharpException> ex = script->invokeUpdate();

		// Check if there was an exception
		if (ex.valid())
		{
			LOG_WARN("Script '{0}' raised an exception!\n Message: '{1}'\n Stack trace:\n{2}",
				script->getName(), ex->getMessage(), ex->getStackTrace());
		}
	}

	return 0;
}

int MonoManager::setStateBundle(void* pState)
{
	m_pStates = static_cast<StateBundle*>(pState);

	// Set fields
	setFields();

	return 0;
}

void MonoManager::initEngine()
{
	// Create domain
	m_appDomain = new SharpDomain("Domain" + m_appDomainIter);

	// Load the engine assembly
	m_engineAssembly = m_appDomain->loadAssembly("", ASSEMBLY_NAME);

	// Assign engine variables
	m_pAderEngine->Klass = m_engineAssembly->getClass(ENGINE_NAMESPACE, ENGINE_CLASS_NAME);
	m_pAderEngine->FieldWndState = m_pAderEngine->Klass->getField("_wndState");
	m_pAderEngine->FieldKeyState = m_pAderEngine->Klass->getField("_keyState");

	// Assign script base variables
	m_pAderScriptBase->Klass = m_engineAssembly->getClass(SCRIPT_NAMESPACE, SCRIPT_CLASS_NAME);
	m_pAderScriptBase->Constructor = m_pAderScriptBase->Klass->getMethod(".ctor", "", false);
	m_pAderScriptBase->Init = m_pAderScriptBase->Klass->getMethod("Init", "", false);
	m_pAderScriptBase->Update = m_pAderScriptBase->Klass->getMethod("Update", "", false);
}

void MonoManager::setFields()
{
	// Arguments array
	void* args[1];

	// Window state
	args[0] = &m_pStates->WndState;
	m_pAderEngine->FieldWndState->setValue(nullptr, args);

	// Key state
	args[0] = &m_pStates->KeyState;
	m_pAderEngine->FieldKeyState->setValue(nullptr, args);
}

SharpDomain::SharpDomain(const std::string& name)
{
	// Create domain
	m_pDomain = mono_domain_create_appdomain(const_cast<char*>(name.c_str()), nullptr);

	// Set the domain
	if (mono_domain_set(m_pDomain, 0))
	{
		mono_thread_attach(m_pDomain);
	}
}

std::vector<Memory::reference<SharpClass>> SharpDomain::getClassInherits(Memory::reference<SharpClass> base)
{
	std::vector<Memory::reference<SharpClass>> result;

	// Iterate over each loaded assembly
	for (const Memory::reference<SharpAssembly>& assembly : m_assemblies)
	{
		// Iterate over all assembly classes
		for (const Memory::reference<SharpClass>& klass : assembly->getAllClasses())
		{
			// Check if class inherits from AderScript
			if (klass->hasBaseClass() && klass->hasBaseClass(base) && klass->getName() != base->getName())
			{
				// Push back the new script
				result.push_back(klass);
			}
		}
	}

	return result;
}

MonoString* SharpDomain::createString(const std::string& from)
{
	return mono_string_new(m_pDomain, from.c_str());
}

Memory::relay_ptr<SharpAssembly> SharpDomain::loadAssembly(const std::string& folder, const std::string& name)
{
	// Create the SharpAssembly
	Memory::reference<SharpAssembly> assembly = new SharpAssembly(m_pDomain, folder, name);

	// Check if it loaded correctly, return nullptr if it didn't
	if (assembly->loaded())
	{
		m_assemblies.push_back(assembly);
		return assembly.asRelay();
	}
	else
	{
		return nullptr;
	}
}

Memory::relay_ptr<SharpAssembly> SharpDomain::getAssembly(const std::string& name)
{
	// Find assemblies where the name matches
	auto it = std::find_if(m_assemblies.begin(), m_assemblies.end(),
		[&](const Memory::reference<SharpAssembly>& assembly)
	{
		return assembly->getName() == name;
	});

	// Check if assembly was found return invalid reference otherwise
	if (it == m_assemblies.end())
	{
		return nullptr;
	}

	// Return reference to the assembly
	return (*it).asRelay();
}

Memory::reference<SharpClass> SharpDomain::getClass(const std::string& nSpace, const std::string& name)
{
	for (const Memory::reference<SharpAssembly>& assembly : m_assemblies)
	{
		Memory::reference<SharpClass> klass = assembly->getClass(nSpace, name);

		if (klass.valid())
		{
			return klass;
		}
	}

	return nullptr;
}

SharpDomain::~SharpDomain()
{
	if (!m_unloaded)
	{
		unload();
	}
}

void SharpDomain::unload()
{
	if (m_unloaded)
	{
		LOG_WARN("Unloading the same domain multiple times!");
		return;
	}

	// Get the root and set it as the main domain
	MonoDomain* root = mono_get_root_domain();
	
	if(mono_domain_set(root, 0))
	{
		// Unload domain
		mono_domain_unload(m_pDomain);

		// Clean all objects
		mono_gc_collect(mono_gc_max_generation());		

		// Clear all loaded assemblies
		// This causes all assemblies to close their images and assemblies
		m_assemblies.clear();
	}

	m_unloaded = true;
}

SharpAssembly::SharpAssembly(MonoDomain* pDomain, const std::string& folder, const std::string& name)
	: m_pDomain(pDomain), m_name(name)
{
	std::string path;

	// Create the path to assembly
	if (folder == "")
	{
		path = name;
	}
	else
	{
		path = folder + "/" + name;
	}

	// Try to load the assembly
	m_pAssembly = mono_domain_assembly_open(m_pDomain, path.c_str());

	// Check if the assembly was loaded correctly
	if (!m_pAssembly)
	{
		LOG_WARN("'{0}' could not be loaded!", path);
		return;
	}

	// Create an image of the assembly
	m_pImage = mono_assembly_get_image(m_pAssembly);

	// Check if the image was created correctly
	if (!m_pImage)
	{
		LOG_WARN("'{0}' image could not be created!", path);
		return;
	}
}

Memory::reference<SharpClass> SharpAssembly::getClass(const std::string& nSpace, const std::string& name) const
{
	// Create the SharpClass
	Memory::reference<SharpClass> klass = new SharpClass(m_pDomain, m_pImage, nSpace, name);

	// Check if it loaded correctly, return nullptr if it didn't
	if (klass->loaded())
	{
		return klass;
	}
	else
	{
		return nullptr;
	}
}

std::vector<Memory::reference<SharpClass>> SharpAssembly::getAllClasses() const
{
	// Vector that will contain all found classes
	std::vector<Memory::reference<SharpClass>> foundClasses;

	// Get meta table info
	const MonoTableInfo* table_info = mono_image_get_table_info(m_pImage, MONO_TABLE_TYPEDEF);

	// Get the amount of rows in the table
	int rows = mono_table_info_get_rows(table_info);

	// Iterate over each row
	for (int i = 0; i < rows; i++)
	{
		uint32_t cols[MONO_TYPEDEF_SIZE];

		// Decode values
		mono_metadata_decode_row(table_info, i, cols, MONO_TYPEDEF_SIZE);

		// Get the name of the class
		std::string name = mono_metadata_string_heap(m_pImage, cols[MONO_TYPEDEF_NAME]);

		// Get the namespace of the class
		std::string name_space = mono_metadata_string_heap(m_pImage, cols[MONO_TYPEDEF_NAMESPACE]);

		if (!SCRIPT_CLASS_FILTER(name))
		{
			Memory::reference<SharpClass> klass = new SharpClass(m_pDomain, m_pImage, name_space, name);

			if (klass->loaded())
			{
				foundClasses.push_back(klass);
			}
		}
	}

	return foundClasses;
}

const std::string& SharpAssembly::getName() const
{
	return m_name;
}

bool SharpAssembly::loaded()
{
	return m_pAssembly != nullptr;
}

SharpClass::SharpClass(MonoDomain* pDomain, MonoImage* pImage, const std::string& nSpace, const std::string& name)
	: m_pDomain(pDomain), m_pImage(pImage)
{
	// Create class
	m_pClass = mono_class_from_name(m_pImage, nSpace.c_str(), name.c_str());

	// Check if the class was created correctly
	if (!m_pClass)
	{
		LOG_WARN("'{0}.{1}' class could not be created. It was not found or doesn't exist!", nSpace, name);
		return;
	}

	m_name = mono_class_get_name(m_pClass);
	m_nSpace = mono_class_get_namespace(m_pClass);
}

SharpClass::SharpClass(MonoDomain* pDomain, MonoImage* pImage, MonoClass* pClass)
	: m_pDomain(pDomain), m_pImage(pImage), m_pClass(pClass)
{
	m_name = mono_class_get_name(m_pClass);
	m_nSpace = mono_class_get_namespace(m_pClass);
}

const std::string& SharpClass::getName()
{
	return m_name;
}

const std::string& SharpClass::getNamespace()
{
	return m_nSpace;
}

bool SharpClass::hasBaseClass()
{
	return mono_class_get_parent(m_pClass) != nullptr;
}

bool SharpClass::hasBaseClass(Memory::reference<SharpClass> base)
{
	return mono_class_is_subclass_of(m_pClass, base->m_pClass, 1) == 1;
}

Memory::reference<SharpClass> SharpClass::getBaseClass()
{
	return new SharpClass(m_pDomain, m_pImage, mono_class_get_parent(m_pClass));
}

void SharpClass::addInternalCall(const std::string& name, const void* callback)
{
	// Construct the signature and map it to the callback
	mono_add_internal_call(methodSignature(name, "", false).c_str(), callback);
}

MonoObject* SharpClass::invokeMethod(const std::string& name, const std::string& signatureParams, void** params)
{
	// Build a method description object
	MonoMethodDesc* methodDesc = mono_method_desc_new(methodSignature(name, signatureParams, true).c_str(), NULL);

	// Search the method in the image
	MonoMethod* method = mono_method_desc_search_in_image(methodDesc, m_pImage);

	// Free memory
	mono_method_desc_free(methodDesc);

	// Invoke the method
	return mono_runtime_invoke(method, nullptr, params, nullptr);
}

std::vector<Memory::reference<SharpMethod>> SharpClass::getAllMethods()
{
	std::vector<Memory::reference<SharpMethod>> methods;

	void* iter = NULL;
	MonoMethod* method;
	while (method = mono_class_get_methods(m_pClass, &iter))
	{
		methods.push_back(new SharpMethod(method));
	}

	return methods;
}

Memory::reference<SharpMethod> SharpClass::getMethod(const std::string& name, const std::string& params, bool isStatic)
{
	// Create the SharpClass
	Memory::reference<SharpMethod> method = new SharpMethod(m_pImage, methodSignature(name, params, isStatic));

	// Check if it loaded correctly, return nullptr if it didn't
	if (method->loaded())
	{
		return method;
	}
	else
	{
		return nullptr;
	}
}

Memory::reference<SharpProperty> SharpClass::getProperty(const std::string& name)
{
	return Memory::reference<SharpProperty>(new SharpProperty(m_pClass, name));
}

Memory::reference<SharpField> SharpClass::getField(const std::string& name)
{
	return Memory::reference<SharpField>(new SharpField(m_pDomain, mono_class_get_field_from_name(m_pClass, name.c_str())));
}

MonoObject* SharpClass::createInstance()
{
	// Create object from class
	return mono_object_new(m_pDomain, m_pClass);
}

bool SharpClass::loaded()
{
	return m_pClass != nullptr;
}

std::string SharpClass::methodSignature(const std::string& name, const std::string& params, bool isStatic)
{
	return getNamespace() + "." + getName() + (isStatic ? ":" : "::") + name + (params.length() > 0 ? "(" + params + ")" : "");
}

SharpMethod::SharpMethod(MonoMethod* pMethod)
	: m_pMethod(pMethod)
{
	if (m_pMethod == nullptr)
	{
		LOG_WARN("Can't create sharp method cause the MonoMethod* is invalid!");
	}

	// Get the full name then copy to string and free the memory
	m_fullName = mono_method_full_name(m_pMethod, 1);
}

SharpMethod::SharpMethod(MonoImage* pImage, const std::string& signature)
{
	//Build a method description object
	MonoMethodDesc* methodDesc = mono_method_desc_new(signature.c_str(), NULL);

	//Search the method in the image
	m_pMethod = mono_method_desc_search_in_image(methodDesc, pImage);

	// Get the full name then copy to string and free the memory
	m_fullName = mono_method_full_name(m_pMethod, 1);

	// Free memory
	mono_method_desc_free(methodDesc);

	// Check if the class was created correctly
	if (!m_pMethod)
	{
		LOG_WARN("'{0}' method could not be created. It was not found or doesn't exist!", signature);
		return;
	}
}

MonoObject* SharpMethod::invokeMethod(MonoObject* pInstance, void** params, MonoObject** exception)
{
	// Invoke the method
	return mono_runtime_invoke(m_pMethod, pInstance, params, exception);
}

MonoObject* SharpMethod::invokeVirtualMethod(MonoObject* pInstance, void** params, MonoObject** exception)
{
	// Get the virtual method and then invoke it
	MonoMethod* virtualMethod = mono_object_get_virtual_method(pInstance, m_pMethod);
	return mono_runtime_invoke(virtualMethod, pInstance, params, exception);
}

Memory::reference<SharpMethod> SharpMethod::getAsVirtual(MonoObject* pInstance)
{
	return new SharpMethod(mono_object_get_virtual_method(pInstance, m_pMethod));
}

void* SharpMethod::getThunk()
{
	return mono_method_get_unmanaged_thunk(m_pMethod);
}

const std::string& SharpMethod::getFullName()
{
	return m_fullName;
}

bool SharpMethod::loaded()
{
	return m_pMethod != nullptr;
}

SharpProperty::SharpProperty(MonoProperty* pProperty)
	: m_pProperty(pProperty), m_getMethod(nullptr), m_setMethod(nullptr)
{
	// Get methods
	MonoMethod* pGet = mono_property_get_get_method(pProperty);
	MonoMethod* pSet = mono_property_get_set_method(pProperty);

	// Check if they exist, if they do create them
	if (pGet)
	{
		m_getMethod = new SharpMethod(pGet);
	}

	if (pSet)
	{
		m_setMethod = new SharpMethod(pSet);
	}
}

SharpProperty::SharpProperty(MonoClass* pClass, const std::string& name)
	: SharpProperty(mono_class_get_property_from_name(pClass, name.c_str()))
{
}

MonoObject* SharpProperty::getValue(MonoObject* pInstance)
{
	ADER_ASSERT(m_getMethod.valid(), "Trying to use invalid property method");
	return m_getMethod->invokeMethod(pInstance, nullptr);
}

void SharpProperty::setValue(MonoObject* pInstance, void** params)
{
	ADER_ASSERT(m_setMethod.valid(), "Trying to use invalid property method");
	m_setMethod->invokeMethod(pInstance, params);
}

void* SharpProperty::getGetThunk()
{
	ADER_ASSERT(m_getMethod.valid(), "Trying to use invalid property method");
	return m_getMethod->getThunk();
}

void* SharpProperty::getSetThunk()
{
	ADER_ASSERT(m_setMethod.valid(), "Trying to use invalid property method");
	return m_setMethod->getThunk();
}

bool SharpProperty::loaded()
{
	return m_pProperty != nullptr;
}

SharpField::SharpField(MonoDomain* pDomain, MonoClassField* pField)
	: m_pField(pField)
{
	// Check if the field is valid
	if (!m_pField)
	{
		LOG_WARN("Couldn't create a field, the passed pointer is invalid!");
		return;
	}

	// Check if the field is static
	uint32_t flags = mono_field_get_flags(m_pField);
	m_isStatic = (flags & MONO_FIELD_ATTR_STATIC) != 0;

	// We have to make sure that the class is initialized if
	// the field is from a static class
	if (m_isStatic)
	{
		// Initialize class that whose field we are trying to access
		m_pVTable = mono_class_vtable(pDomain, mono_field_get_parent(m_pField));
		mono_runtime_class_init(m_pVTable);
	}
}

void SharpField::getValue(MonoObject* pInstance, void* value)
{
	// Check if field is static and set value accordingly
	if (m_isStatic)
	{
		mono_field_static_get_value(m_pVTable, m_pField, value);
	}
	else
	{
		mono_field_get_value(pInstance, m_pField, value);
	}
}

void SharpField::setValue(MonoObject* pInstance, void* value)
{
	// Check if field is static and set value accordingly
	if (m_isStatic)
	{
		mono_field_static_set_value(m_pVTable, m_pField, value);
	}
	else
	{
		mono_field_set_value(pInstance, m_pField, value);
	}
}

bool SharpField::isStatic() const
{
	return m_isStatic;
}

bool SharpField::loaded()
{
	return m_pField != nullptr;
}

SharpException::SharpException(MonoObject* pException)
{
	getValues(pException);
}

const std::string& SharpException::getMessage() const
{
	return m_msg;
}

const std::string& SharpException::getStackTrace() const
{
	return m_stackTrace;
}

void SharpException::getValues(MonoObject* pException)
{
	// Get the class of the exception
	MonoClass* exceptionClass = mono_object_get_class(pException);

	// Create properties
	SharpProperty messageProperty(exceptionClass, "Message");
	SharpProperty stackTraceProperty(exceptionClass, "StackTrace");

	if (messageProperty.loaded() && stackTraceProperty.loaded())
	{
		// Get values
		MonoObject* pMessage = messageProperty.getValue(pException);
		MonoObject* pStackTrace = stackTraceProperty.getValue(pException);

		// Convert to C++ types
		m_msg = SharpMarshal::toString(pMessage);
		m_stackTrace = SharpMarshal::toString(pStackTrace);
	}
	else
	{
		LOG_WARN("Can't find properties for the exception or the object is invalid!");
	}
}

std::string SharpMarshal::toString(MonoObject* pObject)
{
	// Create mono string
	MonoString* pMonoStr = mono_object_to_string(pObject, nullptr);

	// Get the string value
	char* pStr = mono_string_to_utf8(pMonoStr);

	// Create std::string
	std::string str = reinterpret_cast<std::string::value_type*>(pStr);

	// Free the memory of the raw array
	mono_free(pStr);

	return str;
}
