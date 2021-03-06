local engine_paths = require "common"

project "Ader2_SHARP"
	location (engine_paths.project .. "/Ader2_SHARP")
	kind "SharedLib"
	language "C#"

	debugdir (engine_paths.out_target)

	targetdir (engine_paths.out_target)
	objdir (engine_paths.out_object)

	files
	{
		"%{prj.location}/src/**.cs",
	}

	links
	{
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