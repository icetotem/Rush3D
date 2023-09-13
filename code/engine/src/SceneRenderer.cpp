#include "stdafx.h"
#include "SceneRenderer.h"
#include "components/Camera.h"
#include "components/MeshRenderer.h"
#include "components/CommonComponents.h"
#include "render/RMaterial.h"
#include "render/fg/FrameGraph.hpp"
#include "render/fg/Blackboard.hpp"
#include "render/passes/FinalPass.h"
#include "render/passes/UploadFrameBlock.h"

namespace rush
{   

    SceneRenderer::SceneRenderer()
    {

    }

    SceneRenderer::~SceneRenderer()
    {

    }

    void SceneRenderer::Init()
    {
        RenderContext::Init(BackendType::D3D12);
    }

    void SceneRenderer::Update()
    {
        HMap<Camera*, Ref<RenderableHub>> renderContents;

        // fetch culled results
        auto view = EcsSystem::registry.view<InFrustumFlag, MeshRenderer>();
        for (auto [entity, flag, meshRdr] : view.each())
        {
            Entity ent = Entity::Find(entity);
            auto cam = flag.camera.Get<Camera>();
            auto renderer = cam->GetRenderer();
            auto iter = renderContents.find(cam);
            Ref<RenderableHub> contex;
            if (iter == renderContents.end())
            {
                contex = CreateRef<RenderableHub>();
                contex->camera = flag.camera;
                renderContents.insert({ cam, contex });
            }
            else
            {
                contex = iter->second;
            }

            RUSH_ASSERT(contex != nullptr);

            // update global uniforms
            auto globalUniforms = RMaterialInst::GetGlobalUniformBuffer();

            Matrix4 buff[] = { cam->GetViewMatrix(), cam->GetProjMatrix() };
            globalUniforms->UpdateData(buff, sizeof(Matrix4) * 2);

            for (const auto& part : meshRdr.m_Primitives)
            {
                auto batch = contex->NewBatch();
                batch->indexBuffer = part.indexBuffer;
                batch->vertexBuffers = part.vertexBuffers;
                batch->pipeline = part.material->m_Material->m_Pipeline;
                batch->bindGroup = part.material->m_BindGroup;
            }
        }

        // render
        for (auto& [cam, content] : renderContents)
        {
//             auto renderer = cam->GetRenderer();
//             renderer->BeginDraw(cam->GetViewport());
//             renderer->DrawFinalPass(content);
//             renderer->EndDraw();

            FrameGraph fg;
            FrameGraphBlackboard blackboard;

            uint32_t w, h;
            cam->GetViewSize(w, h);

            FrameInfo frameInfo = {
                (float)Timer::GetTimeSec(),
                (float)Timer::GetDeltaTimeSec(),
                Vector2(w, h),
                *cam,
                0, // features
                0 // debug flags
            };
            uploadFrameBlock(fg, blackboard, frameInfo);

            FinalPass finalPass;
            finalPass.compose(fg, blackboard, OutputMode::BaseColor);

            fg.compile();
            fg.execute(&content, &m_transientResources);

            m_transientResources.update(Timer::GetDeltaTimeSec());
        }
    }

    void SceneRenderer::Shutdown()
    {
        RenderContext::Shutdown();
    }

}
