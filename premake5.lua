workspace "Wombat"
	architecture "x64"
	startproject "TestProject"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "Wombat/vendor/glfw/include"
IncludeDir["glm"] = "Wombat/vendor/glm"
IncludeDir["vulkan"] = "Wombat/vendor/Vulkan-Headers/include"

group "Dependencies"
	include "Wombat/vendor/Vulkan-Headers"
	include "Wombat/vendor/glfw"

group ""

project "Wombat"
	location 	"Wombat"
	kind 		"StaticLib"
	language 	"C++"
	cppdialect 	"C++17"
	staticruntime "on"

	targetdir 	("bin/" .. outputdir .. "/%{prj.name}")
	objdir 		("bin-obj/" .. outputdir .. "/%{prj.name}")

	-- source files
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	-- include dirs
	includedirs
	{
		"%{prj.name}/src",
		"%{IncludeDir.vulkan}",
		"%{IncludeDir.GLFW}"

	}

	links
	{
		"GLFW"
	}

	filter "system:windows"
		systemversion 	"latest"

		defines
		{
			"WB_PLATFORM_WINDOWS"
		}
	--windows end

	filter "configurations:Debug"
		defines
		{
			"WB_DEBUG"
		}
		runtime "Debug"
		symbols "on"
	--debug end

	filter "configurations:Release"
		defines
		{
			"WB_RELEASE"
		}
		runtime "Release"
		optimize "on"
	--Release end

	filter "configurations:Dist"
		defines
		{
			"WB_DIST"
		}
		runtime "Release"
		optimize "on"
	--Dist end

project "TestProject"
	location 		"TestProject"
	kind 			"ConsoleApp"
	language 		"C++"
	cppdialect 		"C++17"
	staticruntime 	"on"

	targetdir 	("bin/" .. outputdir .. "/%{prj.name}")
	objdir 		("bin-obj/" .. outputdir .. "/%{prj.name}")
	
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Wombat/src",
		"Wombat/vendor/**"
	}

	links
	{
		"Wombat"
	}

	filter "system:windows"
		systemversion 	"latest"

		defines
		{
			"WB_PLATFORM_WINDOWS"
		}
	--windows end

	filter "configurations:Debug"
		defines
		{
			"WB_DEBUG"
		}
		runtime "Debug"
		symbols "on"
	--debug end

	filter "configurations:Release"
		defines
		{
			"WB_RELEASE"
		}
		runtime "Release"
		optimize "on"
	--Release end

	filter "configurations:Dist"
		defines
		{
			"WB_DIST"
		}
		runtime "Release"
		optimize "on"
	--Dist end