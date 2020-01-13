#pragma once

namespace Messages
{
	/**
	 * All engine messages are defined here
	 *
	 * 0-9   reserved for system messages
	 * 10-19 reserved for window messages
	 * 20-29 reserved for scrip messages
	 * 50-99 reserved for state messages
	 */
	enum Msg
	{
		/**
		 * Message sent after the message bus has initialized all modules.
		 */
		msg_Setup = 0,

		/**
		 * This message is sent every frame and instructs all modules to update
		 * e.g. poll inputs.
		 */
		msg_SystemUpdate = 1,

		/**
		 * This message is sent every frame and instructs all modules to render.
		 * Mostly used to communicate with the specified context module or a rendering API.
		 */
		msg_SystemRender = 2,

		/**
		 * This message is used to request a window creation additional messages
		 * will then be sent by the specif window module that will create the window.
		 */
		msg_CreateWindow = 10,

		/**
		 * This message is sent after a window is created successfully.
		 */
		msg_WindowCreated = 11,

		/**
		 * This message instructs script manager to find all assemblies in a specified directory
		 * and load them in.
		 */
		msg_LoadAssemblies = 20,

		/**
		 * This message instructs script manager to close all current assemblies. This message must
		 * be sent before calling ReloadAssemblies.
		 */
		msg_CloseAssemblies = 21,

		/**
		 * This message instructs script manager to reload all currently loaded assemblies.
		 * This is used to provide the ability to change code compile it and the have the engine
		 * reload it without needing to restart. Before this message all assemblies must be closed.
		 */
		msg_ReloadAssemblies = 22,

		/**
		 * This message instructs script manager to find all script objects in current assemblies.
		 */
		msg_LoadScripts = 23,

		/**
		 * This message instructs script manager to call init on all script objects.
		 */
		msg_InitScripts = 24,

		/**
		 * This message instructs script manager to call update on all script objects.
		 */
		msg_ScriptUpdate = 25,

		/**
		 * This message is sent when all input states are created and the bundle
		 * is being sent.
		 */
		msg_StateBundleCreated = 50,

		/**
		 * This message is sent every time window state has been changed.
		 */
		msg_WndStateUpdated = 51,

		/**
		 * This message is sent every time keyboard state has been changed.
		 */
		msg_KeyStateUpdated = 52,
	};
}