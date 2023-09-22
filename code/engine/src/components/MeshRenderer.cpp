#include "stdafx.h"
#include "components/MeshRenderer.h"
#include "render/Renderer.h"
#include "AssetManager.h"
#include "render/RMesh.h"

namespace rush
{  

    MeshRenderer::MeshRenderer(Entity owner)
        : Component(owner)
    {
    }

    MeshRenderer::~MeshRenderer()
    {
    }

    void MeshRenderer::AddMesh(const StringView& meshPath)
    {
        AssetsManager::instance().LoadMesh(meshPath, [&](AssetLoadResult result, Ref<RMesh> mesh, void*) {
            if (result == AssetLoadResult::Success)
            {
                auto& prim = m_Primitives.emplace_back();
                for (auto& subMesh : mesh->GetSubmeshes())
                {
                    prim.geometry = subMesh.geometry;
                    AssetsManager::instance().LoadMaterial(subMesh.material, [&](AssetLoadResult result, Ref<RMaterial> material, void*) {
                        if (result == AssetLoadResult::Success)
                        {
                            prim.material = material;
                        }
                    });
                }
            }
        });
    }

    void MeshRenderer::SetMaterial(int part, const StringView& material)
    {
        if (part >= 0 && part < m_Primitives.size())
        {
            AssetsManager::instance().LoadMaterial(material, [&](AssetLoadResult result, Ref<RMaterial> material, void*) {
                if (result == AssetLoadResult::Success)
                {
                    m_Primitives[part].material = material;
                }
            });
        }
    }

    void MeshRenderer::SubmitRenderQueue(Ref<RenderQueue> renderQueue)
    {
        for (auto& prim : m_Primitives)
        {
            renderQueue->Add(prim.geometry, prim.material);
        }
    }

}
