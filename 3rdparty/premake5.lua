
DEP_DIR = "./"

project "Rush3D_Deps"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    characterset "MBCS"
    exceptionhandling "On"
	rtti "On"

    defines
    {
        "SPDLOG_COMPILED_LIB", 
        "SPDLOG_NO_EXCEPTIONS", 
        "STB_IMAGE_IMPLEMENTATION", 
        "MAKE_LIB", 
        "B3_USE_CLEW", 
        "BT_USE_SSE_IN_API",
    }

    filter "action:vs*"
        disablewarnings { "4305" }

    files
    {
        path.join(DEP_DIR, "box2d/**"),
        path.join(DEP_DIR, "entt/**"),
        path.join(DEP_DIR, "glm/**"),
        path.join(DEP_DIR, "gltf/**"),
        path.join(DEP_DIR, "json/**"),
        path.join(DEP_DIR, "toml/**"),
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

        path.join(DEP_DIR, "imgui/*.h"),
        path.join(DEP_DIR, "imgui/*.cpp"),

        path.join(DEP_DIR, "libsquish/*.h"),
        path.join(DEP_DIR, "libsquish/*.cpp"),

        path.join(DEP_DIR, "ois/includes/*.h"),
        path.join(DEP_DIR, "ois/src/*.cpp"),

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

        path.join(DEP_DIR, "ozz-animation/include/ozz/animation/runtime/**.h"),
        path.join(DEP_DIR, "ozz-animation/include/ozz/animation/offline/*.h"),
        path.join(DEP_DIR, "ozz-animation/include/ozz/base/**.h"),
        path.join(DEP_DIR, "ozz-animation/src/animation/runtime/**.cc"),
        path.join(DEP_DIR, "ozz-animation/src/animation/offline/*.cc"),
        path.join(DEP_DIR, "ozz-animation/src/base/**.cc"),
    }

    excludes
    {
    }

    includedirs
    {
        "./",
        path.join(DEP_DIR, "ois/includes"),
        path.join(DEP_DIR, "bullet3"),
        path.join(DEP_DIR, "box2d/include"),
        path.join(DEP_DIR, "toml/include"),
        path.join(DEP_DIR, "glm"),
        path.join(DEP_DIR, "lua"),
        path.join(DEP_DIR, "imgui"),
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
        path.join(DEP_DIR, "libsquish"),
        path.join(DEP_DIR, "ozz-animation/include"),
        path.join(DEP_DIR, "ozz-animation/include/ozz"),
        path.join(DEP_DIR, "ozz-animation/src"),
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
            path.join(DEP_DIR, "glfw/src/osmesa_context.c"),
            path.join(DEP_DIR, "ois/includes/win32/*.h"),
            path.join(DEP_DIR, "ois/src/win32/*.cpp")
        }
        defines
        {
            "_GLFW_WIN32",
        }


    project "assimp"
        kind "StaticLib"
        language "C++"
        cppdialect "C++17"
    
        defines
        {
            "RAPIDJSON_HAS_STDSTRING=1",
            "MINIZ_USE_UNALIGNED_LOADS_AND_STORES=0",
            "ASSIMP_IMPORTER_GLTF_USE_OPEN3DGC=1",
            "ASSIMP_BUILD_NO_3DS_EXPORTER",
            "ASSIMP_BUILD_NO_ASSJSON_EXPORTER",
            "ASSIMP_BUILD_NO_M3D_EXPORTER",
            "ASSIMP_BUILD_NO_3MF_EXPORTER",
            "ASSIMP_BUILD_NO_M3D_IMPORTER",
            "ASSIMP_BUILD_NO_X_IMPORTER",
            "ASSIMP_BUILD_NO_3DS_IMPORTER",
            "ASSIMP_BUILD_NO_MD3_IMPORTER",
            "ASSIMP_BUILD_NO_MDL_IMPORTER",
            "ASSIMP_BUILD_NO_MD2_IMPORTER",
            "ASSIMP_BUILD_NO_PLY_IMPORTER",
            "ASSIMP_BUILD_NO_ASE_IMPORTER",
            "ASSIMP_BUILD_NO_AMF_IMPORTER",
            "ASSIMP_BUILD_NO_HMP_IMPORTER",
            "ASSIMP_BUILD_NO_SMD_IMPORTER",
            "ASSIMP_BUILD_NO_MDC_IMPORTER",
            "ASSIMP_BUILD_NO_MD5_IMPORTER",
            "ASSIMP_BUILD_NO_STL_IMPORTER",
            "ASSIMP_BUILD_NO_LWO_IMPORTER",
            "ASSIMP_BUILD_NO_DXF_IMPORTER",
            "ASSIMP_BUILD_NO_NFF_IMPORTER",
            "ASSIMP_BUILD_NO_RAW_IMPORTER",
            "ASSIMP_BUILD_NO_OFF_IMPORTER",
            "ASSIMP_BUILD_NO_AC_IMPORTER",
            "ASSIMP_BUILD_NO_IRRMESH_IMPORTER",
            "ASSIMP_BUILD_NO_IRR_IMPORTER",
            "ASSIMP_BUILD_NO_Q3D_IMPORTER",
            "ASSIMP_BUILD_NO_B3D_IMPORTER",
            "ASSIMP_BUILD_NO_TERRAGEN_IMPORTER",
            "ASSIMP_BUILD_NO_CSM_IMPORTER",
            "ASSIMP_BUILD_NO_3D_IMPORTER",
            "ASSIMP_BUILD_NO_LWS_IMPORTER",
            "ASSIMP_BUILD_NO_OGRE_IMPORTER",
            "ASSIMP_BUILD_NO_OPENGEX_IMPORTER",
            "ASSIMP_BUILD_NO_MS3D_IMPORTER",
            "ASSIMP_BUILD_NO_COB_IMPORTER",
            "ASSIMP_BUILD_NO_BLEND_IMPORTER",
            "ASSIMP_BUILD_NO_Q3BSP_IMPORTER",
            "ASSIMP_BUILD_NO_NDO_IMPORTER",
            "ASSIMP_BUILD_NO_IFC_IMPORTER",
            "ASSIMP_BUILD_NO_XGL_IMPORTER",
            "ASSIMP_BUILD_NO_ASSBIN_IMPORTER",
            "ASSIMP_BUILD_NO_C4D_IMPORTER",
            "ASSIMP_BUILD_NO_3MF_IMPORTER",
            "ASSIMP_BUILD_NO_X3D_IMPORTER",
            "ASSIMP_BUILD_NO_MMD_IMPORTER",
            "ASSIMP_BUILD_NO_STEP_EXPORTER",
            "ASSIMP_BUILD_NO_SIB_IMPORTER",
    
            --"ASSIMP_BUILD_NO_OWN_ZLIB",
            --"ASSIMP_BUILD_NO_EXPORT",
            --"ASSIMP_BUILD_NO_GLTF_EXPORTER",
            --"ASSIMP_BUILD_NO_OBJ_IMPORTER",
            --"ASSIMP_BUILD_NO_BVH_IMPORTER",
            --"ASSIMP_BUILD_NO_COLLADA_IMPORTER",
            -- "ASSIMP_BUILD_NO_FBX_IMPORTER",
            -- "ASSIMP_BUILD_NO_GLTF_IMPORTER",
        -- "ASSIMP_BUILD_NO_MAKELEFTHANDED_PROCESS",
        -- "ASSIMP_BUILD_NO_FLIPUVS_PROCESS",
        -- "ASSIMP_BUILD_NO_FLIPWINDINGORDER_PROCESS",
        -- "ASSIMP_BUILD_NO_CALCTANGENTS_PROCESS",
        -- "ASSIMP_BUILD_NO_JOINVERTICES_PROCESS",
        -- "ASSIMP_BUILD_NO_TRIANGULATE_PROCESS",
        -- "ASSIMP_BUILD_NO_GENFACENORMALS_PROCESS",
        -- "ASSIMP_BUILD_NO_GENVERTEXNORMALS_PROCESS",
        -- "ASSIMP_BUILD_NO_REMOVEVC_PROCESS",
        -- "ASSIMP_BUILD_NO_SPLITLARGEMESHES_PROCESS",
        -- "ASSIMP_BUILD_NO_PRETRANSFORMVERTICES_PROCESS",
        -- "ASSIMP_BUILD_NO_LIMITBONEWEIGHTS_PROCESS",
        -- "ASSIMP_BUILD_NO_VALIDATEDS_PROCESS",
        -- "ASSIMP_BUILD_NO_IMPROVECACHELOCALITY_PROCESS",
        -- "ASSIMP_BUILD_NO_FIXINFACINGNORMALS_PROCESS",
        -- "ASSIMP_BUILD_NO_REMOVE_REDUNDANTMATERIALS_PROCESS",
        -- "ASSIMP_BUILD_NO_FINDINVALIDDATA_PROCESS",
        -- "ASSIMP_BUILD_NO_FINDDEGENERATES_PROCESS",
        -- "ASSIMP_BUILD_NO_SORTBYPTYPE_PROCESS",
        -- "ASSIMP_BUILD_NO_GENUVCOORDS_PROCESS",
        -- "ASSIMP_BUILD_NO_TRANSFORMTEXCOORDS_PROCESS",
        -- "ASSIMP_BUILD_NO_FINDINSTANCES_PROCESS",
        -- "ASSIMP_BUILD_NO_OPTIMIZEMESHES_PROCESS",
        -- "ASSIMP_BUILD_NO_OPTIMIZEGRAPH_PROCESS",
        -- "ASSIMP_BUILD_NO_SPLITBYBONECOUNT_PROCESS",
        -- "ASSIMP_BUILD_NO_DEBONE_PROCESS",
        -- "ASSIMP_BUILD_NO_EMBEDTEXTURES_PROCESS",
        -- "ASSIMP_BUILD_NO_GLOBALSCALE_PROCESS",
        }
        
        files
        {
            path.join(DEP_DIR, "assimp/include/**"),
            path.join(DEP_DIR, "assimp/code/**.cpp"),
            path.join(DEP_DIR, "assimp/Contrib/Clipper/**"),
            path.join(DEP_DIR, "assimp/Contrib/open3dgc/**"),
            path.join(DEP_DIR, "assimp/Contrib/unzip/**"),
            path.join(DEP_DIR, "assimp/Contrib/unzip/**"),
            path.join(DEP_DIR, "assimp/Contrib/zip/src/**.h"),
            path.join(DEP_DIR, "assimp/Contrib/zip/src/**.c"),
            path.join(DEP_DIR, "assimp/Contrib/zlib/**.h"),
            path.join(DEP_DIR, "assimp/Contrib/zlib/**.c"),
        }
    
        excludes
        {
            path.join(DEP_DIR, "assimp/Contrib/zlib/contrib/blast/**"),
            path.join(DEP_DIR, "assimp/Contrib/zlib/contrib/infback9/**"),
            path.join(DEP_DIR, "assimp/Contrib/zlib/contrib/inflate86/**"),
            path.join(DEP_DIR, "assimp/Contrib/zlib/contrib/masmx64/**"),
            path.join(DEP_DIR, "assimp/Contrib/zlib/contrib/minizip/**"),
            path.join(DEP_DIR, "assimp/Contrib/zlib/contrib/testzlib/**"),
            path.join(DEP_DIR, "assimp/Contrib/zlib/contrib/untgz/**"),
        }
    
        includedirs
        {
            path.join(DEP_DIR, "assimp"),
            path.join(DEP_DIR, "assimp/include"),
            path.join(DEP_DIR, "assimp/code"),
            path.join(DEP_DIR, "assimp_config"),
            path.join(DEP_DIR, "assimp/contrib"),
            path.join(DEP_DIR, "assimp/contrib/pugixml/src"),
            path.join(DEP_DIR, "assimp/contrib/zlib"),
            path.join(DEP_DIR, "assimp/contrib/unzip"),
            path.join(DEP_DIR, "assimp/contrib/rapidjson/include"),
        }
    