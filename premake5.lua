workspace "mist-lib"
	toolset "clang"

	configurations { "Debug", "Release" }
	startproject "mist-lib"
	outputdir = "%{cfg.buildcfg}-%{cfg.system}"

	windowwidth = 1280
	windowheight = 720
	windowtitle = "Mist Lib"

prebuildcommands
{
	"{COPYDIR} assets %{cfg.buildtarget.directory}"
}

project "mist-lib"
	kind "ConsoleApp"
	language "C"
	targetdir "bin/%{outputdir}/%{prj.name}"
	objdir "bin/%{outputdir}/%{prj.name}/obj"
	location "."

	defines
	{
		"WINDOW_WIDTH=%{windowwidth}",
		"WINDOW_HEIGHT=%{windowheight}",
		"WINDOW_TITLE=\"%{windowtitle}\"",
		"_PLATFORM_NATIVE"
	}

	links
	{
		"glfw",
		"m",
		"freetype"
	}

	includedirs
	{
		"third-party/cglm/include",
		"third-party/stb/include",
		"third-party/glad/include",
		"third-party/hashmap.c",
		"third-party/cJSON",
		"/usr/include/freetype2",
		"/usr/include/libpng16"
	}

	files 
	{ 
		"src/**.h", "src/**.c",
		"third-party/cglm/src/**.c", "third-party/cglm/include/**.h",
		"third-party/cJSON/cJSON.c", "third-party/cJSON/cJSON.h",
		"third-party/hashmap.c/hashmap.c", "third-party/hashmap.c/hashmap.h",
		"third-party/glad/src/glad.c", "third-party/glad/include/glad/gles2.h",
		"third-party/stb/src/*.c", "third-party/stb/include/*.h",
	}

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

