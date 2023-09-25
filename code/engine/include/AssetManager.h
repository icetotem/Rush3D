#ifndef AssetManager_h__
#define AssetManager_h__

#include "core/Core.h"
#include "render/RBuffer.h"
#include "render/RTexture.h"
#include "render/RMaterial.h"
#include "render/RModel.h"
#include "render/RTexture.h"
#include "render/RShader.h"

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

        void OnLoadFile(const StringView& path);

        void LoadModel(const StringView& path, std::function<void(AssetLoadResult result, Ref<RModel>, void* param)> callback, void* param = nullptr);

        void LoadTexture(const StringView& path, std::function<void(AssetLoadResult result, Ref<RTexture>, void* param)> callback, void* param = nullptr);

        void LoadMaterial(const StringView& path, std::function<void(AssetLoadResult result, Ref<RMaterial>, void* param)> callback, void* param = nullptr);

        void LoadShader(const StringView& path, const StringView& defines, std::function<void(AssetLoadResult result, Ref<RShader>, void* param)> callback, void* param = nullptr);

    private:
        HMap<String, Ref<RModel>> m_Models;
        HMap<String, Ref<RTexture>> m_Textures;
        HMap<String, Ref<RMaterial>> m_Materials;
        Map<uint64_t, Ref<RShader>> m_Shaders;
    };

}

#endif // AssetManager_h__
