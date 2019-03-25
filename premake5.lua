workspace "Ground Station"
	architecture "x64"
	startproject "Ground Station"

	configurations
	{
		"Debug",
		"Release",
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Ground Station/vendor/GLFW/include"
IncludeDir["Glad"] = "Ground Station/vendor/Glad/include"
IncludeDir["ImGui"] = "Ground Station/vendor/imgui"
IncludeDir["glm"] = "Ground Station/vendor/glm"
IncludeDir["Arduino"] = "C:/Users/Troy Neubauer/Documents/Rocketry/Code/Arduino"

include "Ground Station/vendor/GLFW"
include "Ground Station/vendor/Glad"
include "Ground Station/vendor/imgui"

project "Ground Station"
	location "Ground Station"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.Arduino}"
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib",
		"Pdh.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "GS_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "GS_RELEASE"
		runtime "Release"
		optimize "On"

