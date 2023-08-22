
DEP_DIR = "./"

project "Rush3D_Deps"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    exceptionhandling "Off"
	rtti "Off"

    defines{
        "SPDLOG_COMPILED_LIB", "SPDLOG_NO_EXCEPTIONS", "STB_IMAGE_IMPLEMENTATION", "MAKE_LIB", "B3_USE_CLEW", "BT_USE_SSE_IN_API"
    }

    files
    {
        path.join(DEP_DIR, "box2d/**"),
        path.join(DEP_DIR, "entt/**"),
        path.join(DEP_DIR, "glm/**"),
        path.join(DEP_DIR, "gltf/**"),
        path.join(DEP_DIR, "json/**"),
        path.join(DEP_DIR, "LuaBridge/**"),
        path.join(DEP_DIR, "mio/**"),
        path.join(DEP_DIR, "spdlog/**"),
        path.join(DEP_DIR, "stduuid/**"),
        path.join(DEP_DIR, "xxHash/**"),
        path.join(DEP_DIR, "lua/onelua.c"),
        path.join(DEP_DIR, "lua/lua.h"),
        path.join(DEP_DIR, "stb/**"),
        path.join(DEP_DIR, "rttr/**"),
        path.join(DEP_DIR, "cereal/include/**"),
        path.join(DEP_DIR, "pugixml/**"),
        path.join(DEP_DIR, "bullet3/**"),
        path.join(DEP_DIR, "recast/**"),
        path.join(DEP_DIR, "glfw/include/GLFW/glfw3.h"),
        path.join(DEP_DIR, "glfw/include/GLFW/glfw3native.h"),
        path.join(DEP_DIR, "glfw/src/platform.c"),
        path.join(DEP_DIR, "glfw/src/glfw_config.h"),
        path.join(DEP_DIR, "glfw/src/context.c"),
        path.join(DEP_DIR, "glfw/src/init.c"),
        path.join(DEP_DIR, "glfw/src/input.c"),
        path.join(DEP_DIR, "glfw/src/monitor.c"),
        path.join(DEP_DIR, "glfw/src/vulkan.c"),
        path.join(DEP_DIR, "glfw/src/window.c"),
    }

    includedirs
    {
        "./",
        path.join(DEP_DIR, "bullet3"),
        path.join(DEP_DIR, "box2d/include"),
        path.join(DEP_DIR, "glm"),
        path.join(DEP_DIR, "lua"),
        path.join(DEP_DIR, "luabridge/source"),
        path.join(DEP_DIR, "spdlog/include"),
        path.join(DEP_DIR, "xxHash"),
        path.join(DEP_DIR, "stduuid"),
        path.join(DEP_DIR, "stduuid/include"),
        path.join(DEP_DIR, "mio/single_include"),
        path.join(DEP_DIR, "cereal/include"),
        path.join(DEP_DIR, "recast/DebugUtils/include"),
        path.join(DEP_DIR, "recast/Detour/include"),
        path.join(DEP_DIR, "recast/DetourCrowd/include"),
        path.join(DEP_DIR, "recast/DetourTileCache/include"),
        path.join(DEP_DIR, "recast/Recast/include"),
    }

    filter "system:windows"
        files
        {
            path.join(DEP_DIR, "glfw/src/platform.c"),
            path.join(DEP_DIR, "glfw/src/null_init.c"),
            path.join(DEP_DIR, "glfw/src/null_joystick.c"),
            path.join(DEP_DIR, "glfw/src/null_monitor.c"),
            path.join(DEP_DIR, "glfw/src/null_window.c"),
            path.join(DEP_DIR, "glfw/src/win32_init.c"),
            path.join(DEP_DIR, "glfw/src/win32_joystick.c"),
            path.join(DEP_DIR, "glfw/src/win32_monitor.c"),
            path.join(DEP_DIR, "glfw/src/win32_time.c"),
            path.join(DEP_DIR, "glfw/src/win32_thread.c"),
            path.join(DEP_DIR, "glfw/src/win32_window.c"),
            path.join(DEP_DIR, "glfw/src/win32_module.c"),
            path.join(DEP_DIR, "glfw/src/wgl_context.c"),
            path.join(DEP_DIR, "glfw/src/egl_context.c"),
            path.join(DEP_DIR, "glfw/src/osmesa_context.c")
        }
        defines
        {
            "_GLFW_WIN32",
        }
