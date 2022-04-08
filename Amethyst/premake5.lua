workspace "Amethyst"
	architecture "x86_64"
	startproject "AmethystEditor"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Amethyst/vendor/GLFW/include"
IncludeDir["Glad"] = "Amethyst/vendor/Glad/include"
IncludeDir["ImGui"] = "Amethyst/vendor/ImGui"
IncludeDir["Assimp"] = "Amethyst/vendor/Assimp/include"
IncludeDir["glm"] = "Amethyst/vendor/glm"
IncludeDir["stb_image"] = "Amethyst/vendor/stb_image"

group "Dependencies"
	include "Amethyst/vendor/GLFW"
	include "Amethyst/vendor/Glad"
	include "Amethyst/vendor/ImGui"
	include "Amethyst/vendor/Assimp"
group ""

project "Amethyst"
	location "Amethyst"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "amtpch.h"
	pchsource "Amethyst/src/amtpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.Assimp}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}"
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"ImGui",
		"Assimp",
		"opengl32.lib"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"AMT_PLATFORM_WINDOWS",
			"AMT_BUILD_DLL"
		}

	filter "configurations:Debug"
		defines "AMT_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "AMT_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "AMT_DIST"
		runtime "Release"
		optimize "on"

project "AmethystEditor"
	location "AmethystEditor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

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
		"Amethyst/src",
		"Amethyst/vendor",
		"%{IncludeDir.glm}"
	}

	links
	{
		"Amethyst"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"AMT_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "AMT_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "AMT_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "AMT_DIST"
		runtime "Release"
		optimize "on"
