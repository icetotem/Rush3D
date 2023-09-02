ENGINE_DIR = "../Engine"
DEP_DIR = "../../3rdparty"

project "Sample_DrawTriangle"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	exceptionhandling "Off"
	rtti "Off"

    defines 
	{ 
		"__STDC_FORMAT_MACROS", "SPDLOG_NO_EXCEPTIONS" 
	}

    debugdir "../../Bin/%{cfg.buildcfg}"

	files
    {
		path.join("draw_triangle", "**.h"),
		path.join("draw_triangle", "**.inl"),
		path.join("draw_triangle", "**.cpp"),
    }

	includedirs
	{
		DEP_DIR,
		path.join(ENGINE_DIR, "include"),
		path.join(DEP_DIR, "dawn/inc"),
		path.join(DEP_DIR, "spdlog/include"),
		path.join(DEP_DIR, "gainput/include"),
	}

	links
    {
        "Rush3D_Deps", "Rush3D_Engine", "dawn_native", "dawn_proc", "dawncpp"
    }

	filter "system:windows"
		libdirs
		{
			path.join(DEP_DIR, "dawn/lib/win/%{cfg.buildcfg}"),
		}
		links
		{
			"xinput"
		}

project "Sample_BasicFramework"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	exceptionhandling "Off"
	rtti "Off"
		
	defines 
	{ 
		"__STDC_FORMAT_MACROS", "SPDLOG_NO_EXCEPTIONS" 
	}
		
	debugdir "../../Bin/%{cfg.buildcfg}"
		
	files
	{
		path.join("basic_framework", "**.h"),
		path.join("basic_framework", "**.inl"),
		path.join("basic_framework", "**.cpp"),
	}
		
	includedirs
	{
		DEP_DIR,
		path.join(ENGINE_DIR, "include"),
		path.join(DEP_DIR, "dawn/inc"),
		path.join(DEP_DIR, "spdlog/include"),
		path.join(DEP_DIR, "gainput/include"),
	}
		
	links
	{
		"Rush3D_Deps", "Rush3D_Engine", "dawn_native", "dawn_proc", "dawncpp"
	}
	links
	{
		"xinput"
	}
		
	filter "system:windows"
		libdirs
		{
			path.join(DEP_DIR, "dawn/lib/win/%{cfg.buildcfg}"),
		}

project "Sample_LoadMesh"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	exceptionhandling "Off"
	rtti "Off"
		
	defines 
	{ 
		"__STDC_FORMAT_MACROS", "SPDLOG_NO_EXCEPTIONS" 
	}
		
	debugdir "../../Bin/%{cfg.buildcfg}"
		
	files
	{
		path.join("basic_framework", "**.h"),
		path.join("basic_framework", "**.inl"),
		path.join("basic_framework", "**.cpp"),
	}
		
	includedirs
	{
		DEP_DIR,
		path.join(ENGINE_DIR, "include"),
		path.join(DEP_DIR, "dawn/inc"),
		path.join(DEP_DIR, "spdlog/include"),
		path.join(DEP_DIR, "gainput/include"),
	}
		
	links
	{
		"Rush3D_Deps", "Rush3D_Engine", "dawn_native", "dawn_proc", "dawncpp"
	}
		
	filter "system:windows"
		libdirs
		{
			path.join(DEP_DIR, "dawn/lib/win/%{cfg.buildcfg}"),
		}
		links
		{
			"xinput"
		}