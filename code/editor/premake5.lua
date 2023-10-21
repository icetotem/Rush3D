ENGINE_DIR = "../Engine"
DEP_DIR = "../../3rdparty"


project "Rush3D_Editor"
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
		"**.h", "**.cpp"
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
		"Rush3D_Deps", "Rush3D_Engine", "dawn_native", "dawn_proc", "dawncpp", "Shlwapi"
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

