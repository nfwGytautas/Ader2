#include "MonoManager.h"

// For find_if
#include <algorithm>

// Engine messages
#include "Messages.h"

// Logger
#include "Utility/Log.h"


Memory::reference<SharpAssembly> MonoManager::loadAssembly(const std::string& folder, const std::string& name)
{
	// Create the SharpAssembly
	Memory::reference<SharpAssembly> assembly = new SharpAssembly(m_pDomain, folder, name);

	// Check if it loaded correctly, return nullptr if it didn't
	if (assembly->loaded())
	{
		m_assemblies.push_back(assembly);
		return assembly;
	}
	else
	{
		return nullptr;
	}
}

Memory::reference<SharpAssembly> MonoManager::getAssembly(const std::string& name)
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
	return *it;
}

Memory::reference<SharpClass> MonoManager::getClass(const std::string& nSpace, const std::string& name)
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

void MonoManager::reload()
{
	// Clear all loaded assemblies
	m_assemblies.clear();
}

MonoString* MonoManager::createString(const std::string& from)
{
	return mono_string_new(m_pDomain, from.c_str());
}

void MonoManager::addInternalCall(const std::string& name, const void* callback)
{
	// Add a mono internal call
	mono_add_internal_call(name.c_str(), callback);
}

bool MonoManager::canShutdown()
{
	// Currently not blocking
	return true;
}

int MonoManager::onMessage(MessageBus::MessageType msg, MessageBus::DataType pData)
{
	switch (msg)
	{
	case Messages::Setup:
		return setup();
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


	// Create the mono domain
	m_pDomain = mono_jit_init_version("MonoManager", "v4.0.30319");

	return 0;
}

SharpAssembly::SharpAssembly(MonoDomain* pDomain, const std::string& folder, const std::string& name)
	: m_pDomain(pDomain), m_name(name)
{
	std::string path = folder + name;

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
		const char* name = mono_metadata_string_heap(m_pImage, cols[MONO_TYPEDEF_NAME]);

		// Get the namespace of the class
		const char* name_space = mono_metadata_string_heap(m_pImage, cols[MONO_TYPEDEF_NAMESPACE]);

		Memory::reference<SharpClass> klass = new SharpClass(m_pDomain, m_pImage, name_space, name);

		if (klass->loaded())
		{
			foundClasses.push_back(klass);
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

const std::string& SharpClass::getName()
{
	return m_name;
}

const std::string& SharpClass::getNamespace()
{
	return m_nSpace;
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

MonoObject* SharpMethod::invokeMethod(MonoObject* instance, void** params)
{
	// Invoke the method
	return mono_runtime_invoke(m_pMethod, instance, params, nullptr);
}

MonoObject* SharpMethod::invokeVirtualMethod(MonoObject* instance, void** params)
{
	// Get the virtual method and then invoke it
	MonoMethod* virtualMethod = mono_object_get_virtual_method(instance, m_pMethod);
	return mono_runtime_invoke(virtualMethod, instance, params, nullptr);
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
