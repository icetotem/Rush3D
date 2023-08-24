ENGINE_DIR = "./"
DEP_DIR = "../../3rdparty"

project "Rush3D_Engine"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	exceptionhandling "Off"
	rtti "Off"
    defines { "__STDC_FORMAT_MACROS", "SPDLOG_NO_EXCEPTIONS" }

    debugdir "../../Bin/%{cfg.buildcfg}"

	pchheader "stdafx.h"
    pchsource (path.join(ENGINE_DIR, "src/stdafx.cpp"))

	files
    {
		path.join(ENGINE_DIR, "**.h"),
		path.join(ENGINE_DIR, "**.inl"),
		path.join(ENGINE_DIR, "**.cpp"),
    }

	excludes
	{
		
	}

	includedirs
	{
		DEP_DIR,
		path.join(ENGINE_DIR, "include"),
		path.join(DEP_DIR, "dawn/inc"),
		path.join(DEP_DIR, "spdlog/include"),
		path.join(DEP_DIR, "glfw/include"),
	}

	links
    {
        "Rush3D_Deps", "dawn_native", "dawn_platform", "dawn_proc", "dawncpp"
    }

	filter "system:windows"
		libdirs
		{
			path.join(DEP_DIR, "dawn/lib/win/%{cfg.buildcfg}"),
		}


