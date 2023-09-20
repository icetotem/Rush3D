ENGINE_DIR = "./"
DEP_DIR = "../../3rdparty"

project "Rush3D_Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
    characterset "MBCS"
	exceptionhandling "Off"
	rtti "Off"

    defines 
	{ 
		"__STDC_FORMAT_MACROS", "SPDLOG_NO_EXCEPTIONS" 
	}

    debugdir "../../Bin/%{cfg.buildcfg}"

	pchheader "stdafx.h"
    pchsource (path.join(ENGINE_DIR, "src/stdafx.cpp"))

	files
    {
		path.join(ENGINE_DIR, "**.h"),
		path.join(ENGINE_DIR, "**.hpp"),
		path.join(ENGINE_DIR, "**.inl"),
		path.join(ENGINE_DIR, "**.cpp"),
    }

	includedirs
	{
		DEP_DIR,
		path.join(ENGINE_DIR, "include"),
		path.join(DEP_DIR, "dawn/inc"),
		path.join(DEP_DIR, "spdlog/include"),
		path.join(DEP_DIR, "glfw/include"),
		path.join(DEP_DIR, "json"),
		path.join(DEP_DIR, "toml"),
		path.join(DEP_DIR, "stb"),
		path.join(DEP_DIR, "tracy"),
	}

