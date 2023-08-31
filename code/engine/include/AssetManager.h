#ifndef AssetManager_h__
#define AssetManager_h__

#include "core/Core.h"
#include "render/RBuffer.h"
#include "render/RTexture.h"
#include "render/RMaterial.h"
#include "render/RMesh.h"
#include "render/RTexture.h"

namespace rush
{

    enum class AssetLoadResult
    {
        FileNotFound,
        ParseFailed,
        Success,
    };

    /// <summary>
    /// 
    /// </summary>
    class AssetsManager : public Singleton<AssetsManager>
    {
    public:
        AssetsManager();
        ~AssetsManager();

        void Init();

        void ClearRecycle();
        
        void Shutdown();

        void LoadMesh(const StringView& path, std::function<void(AssetLoadResult result, Ref<RMesh>, void* param)> callback, void* param = nullptr);

        void LoadTexture(const StringView& path, std::function<void(AssetLoadResult result, Ref<RTexture>, void* param)> callback, void* param = nullptr);

        void LoadMaterial(const StringView& path, std::function<void(AssetLoadResult result, Ref<RMaterial>, void* param)> callback, void* param = nullptr);

        void LoadMaterialInst(const StringView& path, std::function<void(AssetLoadResult result, Ref<RMaterialInst>, void* param)> callback, void* param = nullptr);

    private:
        HMap<String, Ref<RMesh>> m_Meshes;
        HMap<String, Ref<RTexture>> m_Textures;
        HMap<String, Ref<RMaterial>> m_Materials;
        HMap<String, Ref<RMaterialInst>> m_MaterialInstances;
    };

}

#endif // AssetManager_h__
