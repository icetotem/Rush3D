#include "stdafx.h"
#include "AssetManager.h"
#include "core/Common.h"

namespace rush
{

    AssetsManager* AssetsManager::s_Inst = nullptr;

    AssetsManager::AssetsManager()
    {
        s_Inst = this;
    }

    AssetsManager::~AssetsManager()
    {

    }

    void AssetsManager::Init()
    {
    }

    void AssetsManager::ClearRecycle()
    {

    }

    void AssetsManager::Shutdown()
    {

    }

    void AssetsManager::OnLoadFile(const StringView& path)
    {
        if (Path(String(path)).extension() == ".mat")
        {
            LoadMaterial(path, nullptr, nullptr);
        }
    }

    void AssetsManager::LoadModel(const StringView& path, std::function<void(AssetLoadResult result, Ref<RModel>, void* param)> callback, void* param)
    {
        auto iter = m_Models.find(String(path));
        if (iter == m_Models.end())
        {
            auto model = CreateRef<RModel>();
            model->Load(path);
            m_Models.insert({String(path), model});
            if (callback)
                callback(AssetLoadResult::Success, model, param);
        }
        else
        {
            if (callback)
                callback(AssetLoadResult::Success, iter->second, param);
        }
    }

    void AssetsManager::LoadTexture(const StringView& path, std::function<void(AssetLoadResult result, Ref<RTexture>, void* param)> callback, void* param /*= nullptr*/)
    {
        auto iter = m_Textures.find(String(path));
        if (iter == m_Textures.end())
        {
            auto newTex = CreateRef<RTexture>();
            newTex->Load(path);
            m_Textures.insert({ String(path), newTex });
            if (callback)
                callback(AssetLoadResult::Success, newTex, param);
        }
        else
        {
            if (callback)
                callback(AssetLoadResult::Success, iter->second, param);
        }
    }

    void AssetsManager::LoadMaterial(const StringView& path, std::function<void(AssetLoadResult result, Ref<RMaterial>, void* param)> callback, void* param /*= nullptr*/)
    {
        auto iter = m_Materials.find(String(path));
        if (iter == m_Materials.end())
        {
            auto newMat = CreateRef<RMaterial>();
            newMat->Load(path);
            m_Materials.insert({ String(path), newMat });
            if (callback)
                callback(AssetLoadResult::Success, newMat, param);
        }
        else
        {
            if (callback)
                callback(AssetLoadResult::Success, iter->second, param);
        }
    }

    void AssetsManager::LoadShader(const StringView& path, const StringView& defines, std::function<void(AssetLoadResult result, Ref<RShader>, void* param)> callback, void* param /*= nullptr*/)
    {
        uint64_t h(0);
        hash_combine(h, path);
        hash_combine(h, defines);
        auto iter = m_Shaders.find(h);
        if (iter == m_Shaders.end())
        {
            auto shader = CreateRef<RShader>();
            if (shader->Load(path))
            {
                
                m_Shaders.insert({ h, shader });
                if (callback)
                    callback(AssetLoadResult::Success, shader, param);
            }
            else
            {
                if (callback)
                    callback(AssetLoadResult::ParseFailed, shader, param);
            }
        }
        else
        {
            if (callback)
                callback(AssetLoadResult::Success, iter->second, param);
        }
    }

}