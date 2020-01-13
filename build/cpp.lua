local engine_paths = require "common"

project "Ader2_CPP"
	location (engine_paths.project .. "/Ader2_CPP")
	kind "ConsoleApp"
	language "C++"	
	cppdialect "C++17"
	staticruntime "on"

	debugdir (engine_paths.out_target)

	targetdir (engine_paths.out_target)
	objdir (engine_paths.out_object)

	files
	{
		"%{prj.location}/src/**.h",
		"%{prj.location}/src/**.cpp"
	}

	includedirs
	{
		-- Project files
		"/%{prj.location}/src",

		-- Mono
		engine_paths.lib["mono"]["include"],

		-- spdlog
		engine_paths.lib["spdlog"]["include"],

		-- GLFW
		engine_paths.lib["GLFW"]["include"],

		-- Glad
		engine_paths.lib["Glad"]["include"],
	}

	libdirs
	{
		-- Mono
		engine_paths.lib["mono"]["lib"],

		-- spdlog
		engine_paths.lib["spdlog"]["lib"],

		-- GLFW
		engine_paths.lib["GLFW"]["lib"],

		-- Glad
		engine_paths.lib["Glad"]["lib"],
	}

	links
	{
		-- Mono
		engine_paths.lib["mono"]["dll"],

		-- spdlog
		engine_paths.lib["spdlog"]["dll"],

		-- GLFW
		engine_paths.lib["GLFW"]["dll"],

		-- Glad
		engine_paths.lib["Glad"]["dll"],
	}

	postbuildcommands 
	{
		-- Mono dll
		("{COPY} " .. engine_paths.lib["mono"]["copy_dll"] .. " ../" .. engine_paths.out_target .. "/"),

		-- Mono etc folder
		("{COPY} " .. engine_paths.lib["mono"]["copy_etc"] .. " ../" .. engine_paths.out_target .. "/mono/etc"),

		-- Mono lib folder
		("{COPY} " .. engine_paths.lib["mono"]["copy_lib"] .. " ../" .. engine_paths.out_target .. "/mono/lib"),
	}

	filter "system:windows"
		systemversion "latest"

		links
		{
			"opengl32.lib",
		}

		defines
		{
		}

	filter "configurations:Debug"
		defines 
		{
			"ADER_MACRO_DEBUG"
		}
		runtime "Debug"
		symbols "on"
	
	filter "configurations:Release"
		defines "TR_RELEASE"
		optimize "on"
		runtime "Release"

	filter "configurations:Dist"
		defines "TR_DIST"
		runtime "Release"
		optimize "on"