ENGINE_DIR = "./"
DEP_DIR = "../../3rdparty"

project "Rush3D_Engine"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
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
		path.join(ENGINE_DIR, "**.h"),
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
	}

	links
    {
        "Rush3D_Deps",
    }

	filter "platforms:x86_64"
		libdirs
		{
			path.join(DEP_DIR, "dawn/bin/win/x64/%{cfg.buildcfg}"),
		}


