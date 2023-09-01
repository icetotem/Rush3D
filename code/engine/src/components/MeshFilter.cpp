#include "stdafx.h"
#include "components/MeshFilter.h"
#include "components/MeshRenderer.h"
#include "AssetManager.h"

namespace rush
{

    int MeshFilter::AddPart(const StringView& meshPath, const StringView& matPath)
    {
        auto meshRdr = Get<MeshRenderer>();
        RUSH_ASSERT(meshRdr != nullptr, "MeshFilter depends on MeshRenderer");

        int slot = meshRdr->AddPart();
        AssetsManager::instance().LoadMesh(meshPath, [=](AssetLoadResult result, Ref<RMesh> mesh, void* slot) {
            if (result == AssetLoadResult::Success)
            {
                auto meshRdr = this->Get<MeshRenderer>();
                auto part = meshRdr->GetPart(*(int*)slot);
                for (auto& subMesh : mesh->m_Primitives)
                {
                    part->vertexBuffers = subMesh.vertexBuffers;
                    part->indexBuffer = subMesh.indexBuffer;
                }
            }
        }, &slot);

        AssetsManager::instance().LoadMaterialInst(meshPath, [=](AssetLoadResult result, Ref<RMaterialInst> material, void* slot) {
            if (result == AssetLoadResult::Success)
            {
                auto meshRdr = this->Get<MeshRenderer>();
                auto part = meshRdr->GetPart(*(int*)slot);
                part->material = material;
            }
        }, &slot);

        return slot;
    }

    void MeshFilter::RemovePart(int slot)
    {

    }

}