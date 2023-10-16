#include "stdafx.h"
#include "AssetManager.h"
#include "core/Common.h"
#include "BundleManager.h"
#include <cstdio>

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

    void AssetsManager::DoPreload()
    {
        const auto& shaderAssets = BundleManager::instance().GetFileNamesByExt(".spv");
        for (auto path : shaderAssets)
        {
            auto shader = CreateRef<RShader>(path);
            if (!shader->Load(path))
            {
                LOG_ERROR("Shader load failed {}", path);
            }
            String hash = Path(path).stem().string();
            m_Shaders.insert({path, shader});
        }

        const auto& materialAssets = BundleManager::instance().GetFileNamesByExt(".mat");
        for (auto path : materialAssets)
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
            if (newMat->Load(path))
            {
                m_Materials.insert({ String(path), newMat });
                if (callback)
                    callback(AssetLoadResult::Success, newMat, param);
            }
            else if (callback)
                callback(AssetLoadResult::ParseFailed, newMat, param);
        }
        else
        {
            if (callback)
                callback(AssetLoadResult::Success, iter->second, param);
        }
    }

    void AssetsManager::LoadOrCompileShader(const StringView& path, const List<String>& defines, const StringView& code, const StringView& uniforms, std::function<void(AssetLoadResult result, Ref<RShader>, void* param)> callback, void* param /*= nullptr*/)
    {
        uint64_t h(0);
        hash_combine(h, path);
        hash_combine(h, String(code));
        hash_combine(h, String(uniforms));
        for (const auto& define : defines)
            hash_combine(h, define);
        auto strHash = std::to_string(h);
        auto spvPath = str_tolower("assets/spv/" + Path(path).filename().string() + "." + strHash + ".spv");
        auto iter = m_Shaders.find(spvPath);
        if (iter == m_Shaders.end())
        {
            if (RShader::Compile(path, spvPath, defines, code, uniforms))
            {
                auto relPath = String("../../") + String(spvPath);
                BundleManager::instance().LoadSingleFile(relPath);
                auto shader = CreateRef<RShader>(path);
                if (shader->Load(relPath))
                {
                    m_Shaders.insert({ spvPath, shader });
                    if (callback)
                        callback(AssetLoadResult::Success, shader, param);
                }
                else
                {
                    if (callback)
                        callback(AssetLoadResult::ParseFailed, nullptr, param);
                }
            }
            else
            {
                LOG_ERROR("Compile Shader {} failed", String(path));
                if (callback)
                    callback(AssetLoadResult::ParseFailed, nullptr, param);
            }

        }
        else
        {
            if (callback)
                callback(AssetLoadResult::Success, iter->second, param);
        }
    }

    Ref<RMaterial> AssetsManager::GetMaterial(const StringView& path)
    {
        auto iter = m_Materials.find(String(path));
        if (iter != m_Materials.end())
        {
            return iter->second;
        }
        else
        {
            return nullptr;
        }
    }

}