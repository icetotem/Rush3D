
BUILD_DIR = path.join("./build", _ACTION)

solution "Rush3D"
    location(BUILD_DIR)
    startproject "Rush3D_Engine"
    targetdir "./Bin/%{cfg.buildcfg}"
    configurations { "Release", "Debug" }

    if os.istarget("windows") then
        platforms "x86_64"
    end

    filter "configurations:Release"
        defines
        {
            "NDEBUG",
        }
        optimize "Full"

    filter "configurations:Debug"
        defines
        {
            "_DEBUG",
            "DEBUG",
            "_DEBUG_",
        }
        optimize "Debug"
        symbols "On"

    filter "platforms:x86_64"
        architecture "x86_64"

    filter "action:vs*"
        disablewarnings { "4251", "4996", "4267", "4244", "4291", "4819", "4005", "4065", "4001" }
        
    filter "system:windows"
        defines { "RUSH_PLATFORM_WINDOWS" }
    filter "system:macosx"
        defines { "RUSH_PLATFORM_MAC" }
    filter "system:ios"
        defines { "RUSH_PLATFORM_IOS" }
    filter "system:android"
        defines { "RUSH_PLATFORM_ANDROID" }
    filter "system:linux"
        defines { "RUSH_PLATFORM_LINUX" }

include "3rdparty"
include "code/engine"

group "samples"
include "code/samples/basic_framework"
