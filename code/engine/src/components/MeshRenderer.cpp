#include "stdafx.h"
#include "components/MeshRenderer.h"
#include "render/Renderer.h"
#include "AssetManager.h"
#include "render/RModel.h"

namespace rush
{  

    MeshRenderer::MeshRenderer(Entity owner)
        : Component(owner)
    {
    }

    MeshRenderer::~MeshRenderer()
    {
    }

    void MeshRenderer::AddModel(const StringView& meshPath)
    {
        AssetsManager::instance().LoadModel(meshPath, [&](AssetLoadResult result, Ref<RModel> model, void*) {
            if (result == AssetLoadResult::Success)
            {
                auto& prim = m_Primitives.emplace_back();
                for (const auto& m : model->GeMeshes())
                {
                    for (const auto& p : m.primitives)
                    {
                        prim.geometry = p.geometry;
                        AssetsManager::instance().LoadMaterial(p.material, [&](AssetLoadResult result, Ref<RMaterial> material, void*) {
                            if (result == AssetLoadResult::Success)
                            {
                                prim.material = material;
                            }
                        });
                    }
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
