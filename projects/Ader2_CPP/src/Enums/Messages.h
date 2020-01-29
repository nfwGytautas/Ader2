#pragma once

namespace Messages
{
	/**
	 * All engine messages are defined here
	 *
	 * 0-9		reserved for system messages
	 * 10-19	reserved for window messages
	 * 20-49	reserved for script messages
	 * 50-99	reserved for state messages
	 * 100-199	reserved for context messages
	 * 200-249	reserved for scene messages
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
		 * This message instructs script manager to reload all currently loaded assemblies.
		 * This is used to provide the ability to change code compile it and the have the engine
		 * reload it without needing to restart.
		 */
		msg_ReloadAssemblies = 21,

		/**
		 * This message instructs script manager to find all script objects in current assemblies.
		 */
		msg_LoadScripts = 23,

		/**
		 * This message instructs script manager to find all scene objects in current assemblies.
		 */
		msg_LoadAderScenes = 24,

		/**
		 * This message instructs script manager to call init on all script objects.
		 */
		msg_InitScripts = 25,

		/**
		 * This message instructs script manager to call update on all script objects.
		 */
		msg_ScriptUpdate = 26,

		/**
		 * This message instructs script manager to send all found AderScenes to the engine modules.
		 */
		msg_TransmitScenes = 27,

		/**
		 * This message instructs script manager to send the AderAssets interface pointer.
		 */
		msg_TransmitAssets = 28,

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

		/**
		 * This message is sent when a new scene is set as active scene.
		 */
		msg_SetScene = 200,

		/**
		 * This message instructs SceneManager to load the specified scene. 
		 * Scene manager then invokes the Loader of the scene.
		 */
		msg_LoadScene = 201,

		/**
		 * This message instructs SceneManager to load the current scene. 
		 * Scene manager then loads/reloads all current assets by clearing 
		 * it's visuals and game objects and invoking the Loader again.
		 */
		msg_LoadCurrentScene = 202,

		/**
		 * This message is sent when the current scene has been changed.
		 */
		msg_SceneChanged = 203,

		/**
		 * This message instructs the SceneManager to reload the current scene shaders.
		 */
		msg_ReloadSceneShaders = 204,

		/**
		 * This message instructs the AssetManager to release all assets
		 */
		msg_ClearAssets = 205,
	};
}