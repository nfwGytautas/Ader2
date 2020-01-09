#include <iostream>

#include "AderEngine.h"

MonoString* getMessage()
{
	return mono_string_new(mono_domain_get(), "Hello, world");
}

int main()
{
	AderEngine aEngine;

	Memory::reference<SharpAssembly> assembly = aEngine.scriptManager()->loadAssembly("", "Ader2_SHARP.dll");

	Memory::reference<SharpClass> helloClass = assembly->getClass("Ader2_SHARP", "Hello");

	helloClass->addInternalCall("GetMessage", getMessage);

	for (auto method : helloClass->getAllMethods())
	{
		std::cout << method->getFullName() << std::endl;
	}

	MonoObject* instance1 = helloClass->createInstance();
	MonoObject* instance2 = helloClass->createInstance();

	Memory::reference<SharpMethod> constructor1 = helloClass->getMethod(".ctor", "", false);
	Memory::reference<SharpMethod> constructor2 = helloClass->getMethod(".ctor", "string", false);

	Memory::reference<SharpMethod> outStatic = helloClass->getMethod("OUT", "", true);
	Memory::reference<SharpMethod> outInstance = helloClass->getMethod("OUT_INSTANCE", "", false);

	constructor1->invokeMethod(instance1, nullptr);

	void* args[1];
	args[0] = aEngine.scriptManager()->createString("Test ctor");

	constructor2->invokeMethod(instance2, args);

	outInstance->invokeMethod(instance1, nullptr);
	outInstance->invokeMethod(instance2, nullptr);

	typedef void (*OUT_INSTANCE)(MonoObject* obj);
	OUT_INSTANCE o_func = (OUT_INSTANCE)outInstance->getThunk();

	// Call on first instance
	(*o_func)(instance1);

	// Call on second instance
	(*o_func)(instance2);

	for (int i = 0; i < 1000; i++)
	{
		outInstance->invokeMethod(instance1, nullptr);
		outInstance->invokeMethod(instance2, nullptr);
	}

	for (int i = 0; i < 1000; i++)
	{
		// Call on first instance
		(*o_func)(instance1);

		// Call on second instance
		(*o_func)(instance2);
	}

	return 0;
}