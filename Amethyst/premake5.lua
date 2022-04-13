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
IncludeDir["yamlcpp"] = "Amethyst/vendor/yaml-cpp/include"
IncludeDir["stb_image"] = "Amethyst/vendor/stb_image"
IncludeDir["ImGuizmo"] = "Amethyst/vendor/ImGuizmo"

group "Dependencies"
	include "Amethyst/vendor/GLFW"
	include "Amethyst/vendor/Glad"
	include "Amethyst/vendor/ImGui"
	include "Amethyst/vendor/Assimp"
	include "Amethyst/vendor/yaml-cpp"
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
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/ImGuizmo/**.h",
		"%{prj.name}/vendor/ImGuizmo/**.cpp",
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
		"%{IncludeDir.yamlcpp}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.ImGuizmo}"
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"ImGui",
		"Assimp",
		"yaml-cpp",
		"opengl32.lib"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

	filter "files:Amethyst/vendor/parson/**.c"
	flags { "NoPCH" }

	filter "files:Amethyst/vendor/ImGuizmo/**.cpp"
	flags { "NoPCH" }

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
		"%{IncludeDir.glm}",
		"%{IncludeDir.ImGuizmo}"
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
