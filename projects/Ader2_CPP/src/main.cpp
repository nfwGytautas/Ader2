#include <iostream>

#include "AderEngine.h"

#include "Utility/Timer.h"

MonoString* getMessage()
{
	return mono_string_new(mono_domain_get(), "Hello, world");
}

int main()
{
	AderEngine aEngine;

	// The engine doesn't take away the ability to initialize whenever
	aEngine.postMessage(Messages::msg_Setup);

	// Request a window to be created
	GLWindow::CreateWindowParams params {1280, 720, "Test title"};
	aEngine.postMessage(Messages::msg_CreateWindow, &params);

	// Load assemblies
	std::string codeDir = "logic";
	aEngine.postMessage(Messages::msg_LoadAssemblies, &codeDir);

	// Load all objects that inherit from AderScript
	aEngine.postMessage(Messages::msg_LoadScripts);

	// Load all objects that inherit from AderScene
	aEngine.postMessage(Messages::msg_LoadAderScenes);

	// Init all scripts
	aEngine.postMessage(Messages::msg_InitScripts);

	Utility::Timer timer(false);

	float frameTime = 0.0f;

	bool wFrame = false;

	// Check if the program should exit
	while (!aEngine.shouldClose())
	{
		// Keyboard
		KeyboardState keyboard = aEngine.input()->getKeyState();

		if (keyboard.Keys[InputEnums::KEY_F5])
		{
			aEngine.postMessage(Messages::msg_ReloadAssemblies);
		}

		if (keyboard.Keys[InputEnums::KEY_F6])
		{
			aEngine.postMessage(Messages::msg_ReloadSceneShaders);
		}

		if (keyboard.Keys[InputEnums::KEY_W] && keyboard.Keys[InputEnums::KEY_F])
		{
			wFrame = !wFrame;
			aEngine.context()->toggleWireFrame(wFrame);
		}

		// Start the frame timer
		timer.start();


		// Update the system
		aEngine.postMessage(Messages::msg_SystemUpdate);

		// Prepare for rendering
		aEngine.postMessage(Messages::msg_SystemPreRender);

		// Render the system
		aEngine.postMessage(Messages::msg_SystemRender);

		// Update scripts
		aEngine.postMessage(Messages::msg_ScriptUpdate);


		// Stop the timer
		timer.end();

		// Calculate frame time
		frameTime = timer.microseconds() / 1000 / 1000;

		// Output frame time and FPS
		std::cout << "Frame time: " << frameTime << " s, " << 60 / frameTime << "FPS\r";
	}

	// Shutdown the engine
	aEngine.shutdown();

	return 0;
}