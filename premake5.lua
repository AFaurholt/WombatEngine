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

	project "Wombat"
    -- location         ""
    kind            "StaticLib"
    language        "C++"
    cppdialect      "C++17"
    -- staticruntime   "on"

    targetdir       (bintarget)
    objdir          (binobj)

    files {
        prjsrc .. "**.hpp"
        , prjsrc .. "**.cpp"
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