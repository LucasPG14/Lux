workspace "Amethyst"
	architecture "x64"
	startproject "AmethystEditor"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Amethyst/vendor/GLFW/include"
IncludeDir["Glad"] = "Amethyst/vendor/Glad/include"
IncludeDir["ImGui"] = "Amethyst/vendor/ImGui"

group "Dependencies"
	include "Amethyst/vendor/GLFW"
	include "Amethyst/vendor/Glad"
	include "Amethyst/vendor/ImGui"
group ""

project "Amethyst"
	location "Amethyst"
	kind "SharedLib"
	language "C++"
	staticruntime "Off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "amtpch.h"
	pchsource "Amethyst/src/amtpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}"
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"AMT_PLATFORM_WINDOWS",
			"AMT_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/AmethystEditor/\"")
		}

	filter "configurations:Debug"
		defines "AMT_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "AMT_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		runtime "Release"
		buildoptions "/MD"
		optimize "On"

project "AmethystEditor"
	location "AmethystEditor"
	kind "ConsoleApp"
	language "C++"
	staticruntime "Off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Amethyst/vendor/spdlog/include",
		"Amethyst/src"
	}

	links
	{
		"Amethyst"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"AMT_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "AMT_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "AMT_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "AMT_DIST"
		runtime "Release"
		optimize "On"