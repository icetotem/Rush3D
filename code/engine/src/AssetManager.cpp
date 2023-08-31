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

    void AssetsManager::LoadMesh(const StringView& path, std::function<void(AssetLoadResult result, Ref<RMesh>, void* param)> callback, void* param)
    {
        auto iter = m_Meshes.find(String(path));
        if (iter == m_Meshes.end())
        {
            // TODO: async
            RUSH_ASSERT(callback);
            auto newMesh = CreateRef<RMesh>();
            newMesh->Load(path);
            m_Meshes.insert({String(path), newMesh});
            callback(AssetLoadResult::Success, newMesh, param);
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

    void AssetsManager::LoadMaterialInst(const StringView& path, std::function<void(AssetLoadResult result, Ref<RMaterialInst>, void* param)> callback, void* param /*= nullptr*/)
    {
        auto iter = m_MaterialInstances.find(String(path));
        if (iter == m_MaterialInstances.end())
        {
            // TODO: async
            RUSH_ASSERT(callback);
            auto newMat = CreateRef<RMaterialInst>();
            newMat->Load(path);
            m_MaterialInstances.insert({ String(path), newMat });
            callback(AssetLoadResult::Success, newMat, param);
        }
        else
        {
            callback(AssetLoadResult::Success, iter->second, param);
        }
    }


}