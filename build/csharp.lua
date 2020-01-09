outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Ader2_SHARP"
	location "../Ader2_SHARP"
	kind "SharedLib"
	language "C#"

	targetdir ("../bin/" .. outputdir)
	objdir ("../bin-int/" .. outputdir)

	files
	{
		"../%{prj.name}/src/**.cs"
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