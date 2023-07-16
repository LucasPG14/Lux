workspace "Lux"
	architecture "x86_64"
	startproject "LuxEditor"

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
IncludeDir["GLFW"] = "Lux/vendor/GLFW/include"
IncludeDir["Glad"] = "Lux/vendor/Glad/include"
IncludeDir["ImGui"] = "Lux/vendor/ImGui"
IncludeDir["Assimp"] = "Lux/vendor/Assimp/include"
IncludeDir["glm"] = "Lux/vendor/glm"
IncludeDir["ImGuizmo"] = "Lux/vendor/ImGuizmo"
IncludeDir["Optick"] = "Lux/vendor/Optick/src"
IncludeDir["stb_image"] = "Lux/vendor/stb_image"
IncludeDir["VMA"] = "Lux/vendor/VMA"
IncludeDir["yamlcpp"] = "Lux/vendor/yaml-cpp/include"

group "Dependencies"
	include "Lux/vendor/GLFW"
	include "Lux/vendor/Glad"
	include "Lux/vendor/ImGui"
	include "Lux/vendor/Assimp"
	include "Lux/vendor/yaml-cpp"
	include "Lux/vendor/Optick"
group ""

project "Lux"
	location "Lux"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "luxpch.h"
	pchsource "Lux/src/luxpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
		"%{prj.name}/vendor/ImGuizmo/**.h",
		"%{prj.name}/vendor/ImGuizmo/**.cpp",
		"%{prj.name}/vendor/VMA/include/**.h",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp"
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
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.Optick}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.yamlcpp}"
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"ImGui",
		"Assimp",
		"yaml-cpp",
		"Optick",
		"opengl32.lib"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

	filter "files:Lux/vendor/parson/**.c"
	flags { "NoPCH" }

	filter "files:Lux/vendor/ImGuizmo/**.cpp"
	flags { "NoPCH" }

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"LUX_PLATFORM_WINDOWS",
			"LUX_BUILD_DLL",
		}

	filter "configurations:Debug"
		defines "LUX_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "LUX_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "LUX_DIST"
		runtime "Release"
		optimize "on"

project "LuxEditor"
	location "LuxEditor"
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
		"Lux/vendor/spdlog/include",
		"Lux/src",
		"Lux/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.ImGuizmo}"
	}

	links
	{
		"Lux"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"LUX_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "LUX_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "LUX_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "LUX_DIST"
		runtime "Release"
		optimize "on"

project "LuxOfflineRenderer"
	location "LuxOfflineRenderer"
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
		"Lux/vendor/spdlog/include",
		"Lux/src",
		"Lux/vendor",
		"%{IncludeDir.glm}",
	}

	links
	{
		"Lux"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"LUX_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "LUX_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "LUX_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "LUX_DIST"
		runtime "Release"
		optimize "on"