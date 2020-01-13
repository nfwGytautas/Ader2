local engine_paths = require "common"

project "GameCode"
	location (engine_paths.project .. "/GameCode")
	kind "SharedLib"
	language "C#"

	debugdir (engine_paths.out_target)

	targetdir (engine_paths.out_target .. "/logic")
	objdir (engine_paths.out_object)

	flags { "NoCopyLocal" }

	files
	{
		"%{prj.location}/src/**.cs",
	}

	links
	{
		"Ader2_SHARP",
		"System",
	}

	filter "configurations:Debug"
		defines 
		{
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