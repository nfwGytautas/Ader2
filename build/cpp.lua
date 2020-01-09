outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Ader2_CPP"
	location "../Ader2_CPP"
	kind "ConsoleApp"
	language "C++"	
	cppdialect "C++17"
	staticruntime "on"

	debugdir ("../bin/" .. outputdir)

	targetdir ("../bin/" .. outputdir)
	objdir ("../bin-int/" .. outputdir)

	files
	{
		"../%{prj.name}/src/**.h",
		"../%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		-- Project files
		"../%{prj.name}/src",

		-- Mono
		"../libraries/mono/include/mono-2.0",

		-- spdlog
		"../libraries/spdlog/include/",

	}

	libdirs
	{
		-- Mono
		"../libraries/mono/lib/",

		-- spdlog
		"../libraries/spdlog/lib/",
	}

	links
	{
		-- Mono
		"mono-2.0-sgen.lib",

		-- spdlog
		"r_spdlog.lib",
	}

	postbuildcommands 
	{
		-- Mono dll
		("{COPY} ../libraries/mono/lib/mono-2.0-sgen.dll ../bin/" .. outputdir),

		-- Mono etc folder
		("{COPY} ../libraries/mono/etc ../bin/" .. outputdir.. "/mono/etc"),

		-- Mono lib folder
		("{COPY} ../libraries/mono/lib ../bin/" .. outputdir .. "/mono/lib"),
	}

	filter "system:windows"
		systemversion "latest"

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