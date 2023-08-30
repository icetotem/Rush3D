#include "stdafx.h"
#include "RenderManager.h"
#include "Scene.h"
#include "components/Camera.h"
#include "components/Geometry.h"

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

    }

    void RenderManager::Update()
    {
        HMap<Camera*, Ref<RContent>> renderContents;

        // fetch culled results
        auto view = EcsSystem::registry.view<InFrustumFlag, Geometry>();
        for (auto [entity, flag, geo] : view.each())
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

            auto batch = content->NewBatch();
            batch->indexBuffer = geo.m_IndexBuffer;
            batch->vertexBuffers = geo.m_VertexBuffers;
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

    }

}
