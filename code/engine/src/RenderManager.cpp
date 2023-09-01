#include "stdafx.h"
#include "RenderManager.h"
#include "components/Camera.h"
#include "components/MeshRenderer.h"
#include "components/CommonComponents.h"
#include "render/RMaterial.h"

namespace rush
{   

    RenderManager::RenderManager()
    {

    }

    RenderManager::~RenderManager()
    {

    }

    void RenderManager::Init()
    {
        Renderer::Init(RenderBackend::D3D12);
    }

    void RenderManager::Update()
    {
        HMap<Camera*, Ref<RContent>> renderContents;

        // fetch culled results
        auto view = EcsSystem::registry.view<InFrustumFlag, MeshRenderer>();
        for (auto [entity, flag, meshRdr] : view.each())
        {
            Entity ent = Entity::Find(entity);
            auto cam = flag.m_ByCamera.Get<Camera>();
            auto renderer = cam->GetRenderer();
            auto iter = renderContents.find(cam);
            Ref<RContent> content;
            if (iter == renderContents.end())
            {
                content = CreateRef<RContent>();
                renderContents.insert({cam, content });
            }
            else
            {
                content = iter->second;
            }

            RUSH_ASSERT(content != nullptr);

            for (const auto& part : meshRdr.m_Primitives)
            {
                auto batch = content->NewBatch();
                batch->indexBuffer = part.indexBuffer;
                batch->vertexBuffers = part.vertexBuffers;
                batch->pipeline = part.material->m_Material->m_Pipeline;
                batch->bindGroup = part.material->m_BindGroup;
            }
        }

        // render
        for (auto& [cam, content] : renderContents)
        {
            auto renderer = cam->GetRenderer();
            renderer->BeginDraw(cam->GetViewport());
            renderer->DrawFinalPass(content);
            renderer->EndDraw();
        }
    }

    void RenderManager::Shutdown()
    {
        Renderer::Shutdown();
    }

}
