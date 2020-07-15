workspace "WombatEngine"
    architecture "x64"
    configurations { "Debug", "Release" }

-- vars
    buildname   = "%{prj.name}-%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/"
    bintarget   = "bin-target/" .. buildname
    binobj      = "bin-obj/" .. buildname
    prjsrc      = "%{prj.name}/src/"
    prjinc      = "%{prj.name}/include/"
	objcopyx64  = "objcopy --input binary --output-target pe-x86-64 --binary-architecture i386:x86-64 "
	submoddir	= "Wombat/vendor/"
	linkedlibs	= {}
	linkedlibs["GLFW"] 		= submoddir .. "glfw/"
	linkedlibs["vulkan"]	= submoddir .. "vulkan/"
	
project "Wombat"
    -- location         ""
    kind            "StaticLib"
    language        "C++"
    cppdialect      "C++17"
    staticruntime   "on"

    targetdir       (bintarget)
    objdir          (binobj)

    files {
        prjsrc .. "**.hpp"
        , prjsrc .. "**.cpp"
	}
	
	includedirs {
		linkedlibs["GLFW"] .. "include"
		, linkedlibs["vulkan"] .. "include"
	}

	links {
		"GLFW"
	}

filter "system:windows"
    systemversion   "latest"
    defines {
        "WOMBAT_WINDOWS"
    }

filter "configurations:Debug"
    defines {
        "WOMBAT_DEBUG"
    }
    runtime "Debug"
    symbols "On"

filter "configurations:Release"
    defines {
        "WOMBAT_RELEASE"
    }
    runtime "Release"
	optimize "Full"
	
project "GLFW"
	kind "StaticLib"
	language "C"
	staticruntime "on"

	targetdir	(bintarget)
	objdir		(binobj)

	files{
		linkedlibs["GLFW"] .. "include/GLFW/glfw3.h"
		, linkedlibs["GLFW"] .. "include/GLFW/glfw3native.h"
		, linkedlibs["GLFW"] .. "src/internal.h"
		, linkedlibs["GLFW"] .. "src/mappings.h"
		, linkedlibs["GLFW"] .. "src/context.c"
		, linkedlibs["GLFW"] .. "src/init.c"
		, linkedlibs["GLFW"] .. "src/input.c"
		, linkedlibs["GLFW"] .. "src/monitor.c"
		, linkedlibs["GLFW"] .. "src/vulkan.c"
		, linkedlibs["GLFW"] .. "src/window.c"
	}

	defines{
		"_GLFW_VULKAN_STATIC"
	}

filter "system:windows"
	systemversion "latest"

	defines{
		"_GLFW_WIN32"
	}

	files{
		linkedlibs["GLFW"] .. "src/win32_platform.h"
		, linkedlibs["GLFW"] .. "src/win32_joystick.h"
		, linkedlibs["GLFW"] .. "src/wgl_context.h"
		, linkedlibs["GLFW"] .. "src/egl_context.h"
		, linkedlibs["GLFW"] .. "src/osmesa_context.h"
		, linkedlibs["GLFW"] .. "src/win32_init.c"
		, linkedlibs["GLFW"] .. "src/win32_joystick.c"
		, linkedlibs["GLFW"] .. "src/win32_monitor.c"
		, linkedlibs["GLFW"] .. "src/win32_time.c"
		, linkedlibs["GLFW"] .. "src/win32_thread.c"
		, linkedlibs["GLFW"] .. "src/win32_window.c"
		, linkedlibs["GLFW"] .. "src/wgl_context.c"
		, linkedlibs["GLFW"] .. "src/egl_context.c"
		, linkedlibs["GLFW"] .. "src/osmesa_context.c"
	}

filter "configurations:Debug"
	runtime "Debug"
	symbols "on"

filter "configurations:Release"
	runtime "Release"
	optimize "on"