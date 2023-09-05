#include "stdafx.h"
#include "Engine.h"
#include "Window.h"
#include "render/Renderer.h"
#include "render/RTexture.h"
#include "render/RBuffer.h"
#include "render/RShader.h"
#include "render/RUniform.h"
#include "render/RPipeline.h"
#include "render/RBatch.h"
#include "components/EcsSystem.h"
#include "components/Transform.h"
#include "components/MeshRenderer.h"
#include "components/Camera.h"
#include "components/Bounding.h"
#include "components/Frustum.h"
#include "components/MeshFilter.h"
#include "components/CameraCtrl.h"

using namespace rush;

int main(int argc, char* argv[])
{
    Engine engine;
    engine.Init();

    engine.m_BundleManager.AddFolder("../../assets");

    WindowDesc wndDesc;
    wndDesc.title = "RushDmeo";
    wndDesc.width = 1024;
    wndDesc.height = 1024;
    wndDesc.alwaysTop = false;
    wndDesc.visible = false;

    RendererDesc rendererDesc;
    rendererDesc.clearColor = Vector4(0.0f, 0.0f, 0.2f, 1.0f);

    auto window = engine.CreateRenderWindow(wndDesc, rendererDesc);

    if (window == nullptr)
        return -1;

    window->Show(true);

    window->BindKeyCallback([=](InputButtonState state, KeyCode code)->bool{
        if (state == InputButtonState::Pressed)
        {
            if (code == KeyCode::Escape)
            {
                window->Close();
            }
        }
        return false;
    });

    // create camera
    {
        auto camEnt = Entity::Create();

        auto trans = camEnt.Add<Transform>();
        trans->SetPosition(1, 1, 1);
        trans->LookAt(0, 0, 0);

        auto cam = camEnt.Add<Camera>();
        cam->SetRenderer(window->GetRenderer());

        auto frustum = camEnt.Add<Frustum>();

        auto ctrl = camEnt.Add<CameraCtrlStandard>();

    }

    // create the entity
    {
        auto ent0 = Entity::Create();

        auto trans = ent0.Add<Transform>();
        trans->SetPosition(0, 0, 0);

        ent0.Add<MeshRenderer>();
        auto meshFilter = ent0.Add<MeshFilter>();
        meshFilter->AddPart("assets/monkey.glb", "mat_name");

        auto bonding = ent0.Add<Bounding>();
    }

    while (window->ShouldClose())
    {
        Window::MessgeLoop();
        engine.Update();

        auto view = EcsSystem::registry.view<CameraCtrlStandard, Camera>();
        for (auto [EntityID, ctrl, camera] : view.each())
        {
            //ctrl.update(Timer::GetDeltaTimeSec(), false);
            
        }

        window->Present();
    }


    engine.Shutdown();

    return 0;
}
