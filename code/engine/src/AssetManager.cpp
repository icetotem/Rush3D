#include "stdafx.h"
#include "AssetManager.h"

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

    void AssetsManager::LoadModel(const StringView& path, std::function<void(AssetLoadResult result, Ref<RModel>, void* param)> callback, void* param)
    {
        auto iter = m_Models.find(String(path));
        if (iter == m_Models.end())
        {
            // TODO: async
            RUSH_ASSERT(callback);
            auto model = CreateRef<RModel>();
            model->Load(path);
            m_Models.insert({String(path), model});
            callback(AssetLoadResult::Success, model, param);
        }
        else
        {
            callback(AssetLoadResult::Success, iter->second, param);
        }
    }

    void AssetsManager::LoadTexture(const StringView& path, std::function<void(AssetLoadResult result, Ref<RTexture>, void* param)> callback, void* param /*= nullptr*/)
    {
        auto iter = m_Textures.find(String(path));
        if (iter == m_Textures.end())
        {
            // TODO: async
            RUSH_ASSERT(callback);
            auto newTex = CreateRef<RTexture>();
            newTex->Load(path);
            m_Textures.insert({ String(path), newTex });
            callback(AssetLoadResult::Success, newTex, param);
        }
        else
        {
            callback(AssetLoadResult::Success, iter->second, param);
        }
    }

    void AssetsManager::LoadMaterial(const StringView& path, std::function<void(AssetLoadResult result, Ref<RMaterial>, void* param)> callback, void* param /*= nullptr*/)
    {
        auto iter = m_Materials.find(String(path));
        if (iter == m_Materials.end())
        {
            // TODO: async
            RUSH_ASSERT(callback);
            auto newMat = CreateRef<RMaterial>();
            newMat->Load(path);
            m_Materials.insert({ String(path), newMat });
            callback(AssetLoadResult::Success, newMat, param);
        }
        else
        {
            callback(AssetLoadResult::Success, iter->second, param);
        }
    }


}