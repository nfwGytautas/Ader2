#include "AderScript.h"

AderScript::AderScript(AderScriptBase* base, Memory::reference<SharpClass> klass)
	: m_class(klass)
{
	// Create instance
	m_pInstance = m_class->createInstance();

	// Create constructor method
	m_constructor = m_class->getMethod(".ctor", "", false);

	// Create Init method
	m_init = base->Init->getAsVirtual(m_pInstance);

	// Create virtual update method
	m_update = base->Update->getAsVirtual(m_pInstance);

	// Create thunk from update method
	m_thunk_update = (thunk_update)m_update->getThunk();
}

const std::string& AderScript::getName() const
{
	return m_class->getName();
}

void AderScript::invokeConstruct()
{
	m_constructor->invokeMethod(m_pInstance, nullptr);
}

Memory::reference<SharpException> AderScript::invokeInit()
{
	// Invoke init
	MonoObject* exception;
	m_init->invokeVirtualMethod(m_pInstance, nullptr, &exception);

	// Check for exception
	if (exception)
	{
		return new SharpException(exception);
	}
	else
	{
		return nullptr;
	}
}

Memory::reference<SharpException> AderScript::invokeUpdate()
{
	// Invoke method
	MonoObject* exception;
	m_thunk_update(m_pInstance, &exception);
	
	// Check for exception
	if (exception)
	{
		return new SharpException(exception);
	}
	else
	{
		return nullptr;
	}
}
