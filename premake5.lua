require("premake-emcc/emscripten")

workspace "mist-lib"
	startproject "mist-lib"

	configurations { "Debug", "Release" }
	platforms { "Native", "Web" }

	filter { "platforms:Native" }
		toolset "clang"
	filter { "platforms:Web" }
	    toolset "emcc"

	outputdir = "%{cfg.buildcfg}-%{cfg.platform}"

	window_width = 1280
	window_height = 720
	window_title = "Mist Lib"
	assets_dir = "assets"

project "mist-lib"	
	kind "ConsoleApp"
	language "C"
	targetdir "bin/%{outputdir}/%{prj.name}"
	objdir "bin/%{outputdir}/%{prj.name}/obj"
	location "."

	prebuildcommands
	{
		"{COPYDIR} assets %{cfg.buildtarget.directory}",	
	}

	filter { "platforms:Native" }
		defines
		{
			"_PLATFORM_NATIVE"
		}

		links
		{
			"glfw",
		}

		includedirs
		{
			"/usr/include/freetype2",
			"/usr/include/libpng16"
		}
	filter{}

	filter { "platforms:Web" }
		targetextension ".html"

		defines
		{
			"_PLATFORM_WEB"
		}

		links
		{
			"glfw3"
		}

		buildoptions
		{
			"-sUSE_FREETYPE=1",
		}

		linkoptions
		{
			"-sUSE_GLFW=3",
			"-sWASM=1",
			"-sMAX_WEBGL_VERSION=2",
			"-sALLOW_MEMORY_GROWTH=1",
			"-sNO_EXIT_RUNTIME=0",
			"-sASSERTIONS=1",
			"--preload-file %{assets_dir}",
			"--shell-file %{assets_dir}/index.html",
		}
	filter {}

	defines
	{
		"WINDOW_WIDTH=%{window_width}",
		"WINDOW_HEIGHT=%{window_height}",
		"WINDOW_TITLE=\"%{window_title}\"",
	}

	links
	{
		"m",
		"freetype",
		"stdc++",
		"cimgui",
	}

	includedirs
	{
		"third-party/cglm/include",
		"third-party/stb/include",
		"third-party/glad/include",
		"third-party/hashmap.c",
		"third-party/cJSON",
		"third-party/cimgui",
		"third-party/cimgui/imgui",
		"third-party/cimgui/imgui/backends",
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

project "cimgui"
	kind "StaticLib"
	language "C++"
	targetdir "bin/%{outputdir}/%{prj.name}"
	objdir "bin/%{outputdir}/%{prj.name}/obj"
	location "third-party/cimgui"

	includedirs
	{
		"third-party/cimgui/imgui",
		"third-party/cimgui/imgui/backends",
	}

	files
	{
		"third-party/cimgui/cimgui.h",
		"third-party/cimgui/cimgui.cpp",
		"third-party/cimgui/cimgui_impl_opengl3.h",
		"third-party/cimgui/cimgui_impl_opengl3.cpp",
		"third-party/cimgui/cimgui_impl_glfw.h",
		"third-party/cimgui/cimgui_impl_glfw.cpp",
		"third-party/cimgui/imgui/imgui.cpp",
		"third-party/cimgui/imgui/imgui_demo.cpp",
		"third-party/cimgui/imgui/imgui_draw.cpp",
		"third-party/cimgui/imgui/imgui_tables.cpp",
		"third-party/cimgui/imgui/imgui_widgets.cpp",
		"third-party/cimgui/imgui/imconfig.h",
		"third-party/cimgui/imgui/backends/imgui_impl_opengl3.h",
		"third-party/cimgui/imgui/backends/imgui_impl_opengl3.cpp",
		"third-party/cimgui/imgui/backends/imgui_impl_glfw.h",
		"third-party/cimgui/imgui/backends/imgui_impl_glfw.cpp",
	}

