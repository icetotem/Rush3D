#include "stdafx.h"
#include "Engine.h"
#include "Window.h"
#include "components/EcsSystem.h"
#include "components/Transform.h"
#include "components/MeshRenderer.h"
#include "components/Camera.h"
#include "CameraCtrl.h"

using namespace rush;

int main(int argc, char* argv[])
{
    WindowDesc wndDesc;
    wndDesc.title = "RushDmeo";
    wndDesc.posX = 200;
    wndDesc.posY = 200;
    wndDesc.width = 1280;
    wndDesc.height = 720;
    wndDesc.alwaysTop = false;
    wndDesc.visible = true;
    wndDesc.frameless = false;
    wndDesc.vsync = true;
    wndDesc.resizable = true;
    wndDesc.transparent = false;
    wndDesc.windowMode = WindowMode::Windowed;

    Engine engine;

    engine.GetBundleManager().AddFolder("../../assets");

    engine.Init(wndDesc);


    auto mainScene = engine.GetSceneManager().GetMainScene();
    auto mainCamera = mainScene->GetMainCamera();

    // directional light
    {
        auto dirLight = mainScene->CreateEntity("dirLight");
        auto light = dirLight.Add<Light>();
        light->SetType(LightType::LT_Directional);
        light->SetColor(Vector3(0.15f, 0.15f, 0.15f));
        light->SetIntensity(1.0f);
        auto transform = dirLight.Add<Transform>();
        transform->SetPosition(1, 1, 1);
        transform->LookAt(0, 0, 0);
    }
    // point light #1
    {
        auto pointLight = mainScene->CreateEntity("pointLight#1");
        auto light = pointLight.Add<Light>();
        light->SetType(LightType::LT_Point);
        light->SetColor(Vector3(0.15f, 0.15f, 0.15f));
        light->SetIntensity(1.0f);
        light->SetRadius(1.0f);
        auto transform = pointLight.Add<Transform>();
        transform->SetPosition(0, 0, 0);
    }

    CameraCtrlFirstPerson firstPersonCtrl;
    firstPersonCtrl.Setup(engine.GetMainWindow(), mainCamera);

    // create the entity
    {
        auto ent0 = mainScene->CreateEntity("ent0");

        auto trans = ent0.Add<Transform>();
        trans->SetPosition(0, 0, 0);

        auto meshRenderer = ent0.Add<MeshRenderer>();
        meshRenderer->AddModel("assets/cube.glb");
        //meshRenderer->SetMaterial(0, "assets/test.mat");

        auto bonding = ent0.Add<Bounding>();
    }

    while (engine.GetMainWindow()->ShouldClose())
    {
        engine.Update();
        firstPersonCtrl.Update();
    }

    engine.Shutdown();

    return 0;
}
