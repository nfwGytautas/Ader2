#pragma once

#include <CommonTypes/reference.h>
#include <vector>

// Forward declarations
class ModuleSystem;
class MessageBus;
class Module;


/**
 * MessageBus class is used to provide communication for Modules inside a system.
 * Every Module has the ability to post a message and then it's up to the MessageBus
 * to decide what to do to the message, propagate or ignore it.
 */
class MessageBus
{
public:
	/// The type of all messages
	using MessageType = size_t;
	/// The type of all message data
	using DataType = void*;
	/// The type that all modules are stored in
	using ModuleEntry = Memory::reference<Module>;
public:
	/**
	 * Virtual destructor
	 */
	virtual ~MessageBus() {}

	/**
	 * Method used to specify if the message bus can safely be shut down and destroyed
	 *
	 * @return True if the message bus can be destroyed, False otherwise
	 */
	virtual bool canShutdown() = 0;

	/**
	 * Method used to shutdown the Modules and the MessageBus, use in conjunction with canShutdown
	 * to specify if the Modules/MessageBus is ready to be shutdown
	 */
	virtual void shutdown() = 0;

	/**
	 * postMessage method is responsible for handling all messages that are sent
	 * either from the ModuleSystem or from modules. All of which end up here in
	 * this method and then with custom logic they can be filtered.
	 *
	 * @param Msg Message being sent
	 * @param pData Pointer to the data of the message it must be valid until
	 *              the message is no longer needed
	 */
	virtual void postMessage(MessageType msg, DataType pData = nullptr) = 0;

	/**
	 * This method is used to initialize all modules by setting the their message bus.
	 * This can be overridden if extra functionality is needed e.g. sending a message 
	 * after all modules are initialized. By default this method assigns prtc_modules 
	 * variable to the passed modules vector and then iterates over these modules
	 * and calls setMsgBus on each of them.
	 *
	 * @param modules The vector of all modules that needs to be initialized.
	 *                NOTE: Message bus modules will be set to the ones that have been passed.
	 */
	virtual void initModules(std::vector<ModuleEntry> modules);
protected:
	/// All modules that are connected to the message bus
	std::vector<ModuleEntry> prtc_modules;
};


/**
 * Module class provides a common interfaces for decoupling different parts of the program
 * using a message bus and methods for sending said messages. Modules should at least not have
 * any dependencies on other Modules and should only use the onMessage and postMessage methods.
 */
class Module
{
public:
	/**
	 * Virtual destructor
	 */
	virtual ~Module() {}

	/**
	 * Method used to specify if the module can safely be shut down and destroyed
	 *
	 * @return True if the message bus can be destroyed, False otherwise
	 */
	virtual bool canShutdown() = 0;

	/**
	 * Method used to shutdown the Module, use in conjunction with canShutdown
	 * to specify if the Module is ready to be shutdown
	 */
	virtual void shutdown() = 0;

	/**
	 * onMessage method is responsible for handling messages that are sent from
	 * the message bus. This method should return a result of the method that
	 * specifies what the message bus should do after the call. These results
	 * should be matched with message bus implementation.
	 *
	 * @param Msg Message being sent
	 * @param pData Pointer to the data of the message it must be valid until
	 *              the message is no longer needed
	 *
	 * @return The result of this message e.g should the message bus propagate further
	 */
	virtual int onMessage(MessageBus::MessageType msg, MessageBus::DataType pData) = 0;

	/**
	 * This method is used to send a message to the message bus.
	 *
	 * @param Msg Message being sent
	 * @param pData Pointer to the data of the message it must be valid until
	 *              the message is no longer needed
	 */
	void postMessage(MessageBus::MessageType msg, MessageBus::DataType pData = nullptr);

	/**
	 * Sets message bus used for this Module
	 *
	 * @param pMsgBus Pointer to the message bus implementation
	 */
	void setMsgBus(MessageBus* pMsgBus);
private:
	/// Pointer to the message bus implementation
	MessageBus* m_pMsgBus;
};


/**
 * ModuleSystem class provides a interface for setting up a message bus and decoupled modules
 * by providing a common way to get modules of the system and the message bus implementation.
 */
class ModuleSystem
{
public:
	/**
	 * Virtual destructor
	 */
	virtual ~ModuleSystem() {}

	/**
	 * This method is used to check if all modules and the message bus can safely shutdown and
	 * be destroyed
	 *
	 * @return True if all Modules::canShutdown and MessageBus::canShutdown return True, False otherwise
	 */
	bool canShutdown();

	/**
	 * This method is used to shutdown the entire system including the MessageBus
	 */
	void shutdown();

	/**
	 * getModules method is used to get all the modules that are used by the system.
	 * These modules must be valid references
	 *
	 * @return Vector of references to modules used by this System
	 */
	virtual std::vector<MessageBus::ModuleEntry> getModules() = 0;

	/**
	 * getMBImplementation method is used to get the message bus implementation for this system.
	 *
	 * @return Pointer to the implementation of the message bus
	 */
	virtual MessageBus* getMBImplementation() = 0;

	/**
	 * This method is used for initializing the system by setting up the message bus
	 * and initializing all modules that are used by this system
	 */
	void moduleSysInit();

	/**
	 * Similarly to the Module::postMessage this method is used to send a message to the message bus.
	 *
	 * @param Msg Message being sent
	 * @param pData Pointer to the data of the message it must be valid until
	 *              the message is no longer needed
	 */
	void postMessage(MessageBus::MessageType msg, MessageBus::DataType pData = nullptr);
private:
	/// Reference to the message bus implementation
	Memory::reference<MessageBus> m_MsgBus;
};
