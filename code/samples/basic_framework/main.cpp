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
    if (1)
    {
        auto dirLight = mainScene->CreateEntity("dirLight");
        auto light = dirLight.Add<Light>();
        light->SetType(LightType::LT_Directional);
        light->SetColor(Vector3(1.0f));
        //light->SetIntensity(1.f);
        auto transform = dirLight.Add<Transform>();
        transform->SetPosition(2, 2, 2);
        transform->LookAt(0, 0, 0);
    }
    // point lights
//     for (int i = -3; i <= 3; ++i)
//     {
//         auto pointLight = mainScene->CreateEntity();
//         auto light = pointLight.Add<Light>();
//         light->SetType(LightType::LT_Point);
//         light->SetColor(Vector3(1.0f, 0.5f, 0.5f));
//         light->SetIntensity(10.5F);
//         light->SetRadius(15.0f);
//         auto transform = pointLight.Add<Transform>();
//         transform->SetPosition(i * 3.0f, 1.5, 0);
//     }

    // spot light #1
    for (int i = -3; i <= 3; ++i)
    {
        auto pointLight = mainScene->CreateEntity();
        auto light = pointLight.Add<Light>();
        light->SetType(LightType::LT_Spot);
        light->SetColor(Vector3(0.0f, 1.0f, 0.5f));
        light->SetIntensity(5.5F);
        light->SetRadius(1.1f);
        light->SetInerAngle(0.0f);
        light->SetOutterAngle(0.0f);
        auto transform = pointLight.Add<Transform>();
        transform->SetPosition(i * 3.0f, 0, 2.0);
        transform->LookAt(0, 0, 0);
    }

    CameraCtrlFirstPerson firstPersonCtrl;
    firstPersonCtrl.Setup(engine.GetMainWindow(), mainCamera);

    // create the entity
    for (int i = -3; i <= 3; ++i)
    {
        auto ent = mainScene->CreateEntity();

        auto trans = ent.Add<Transform>();
        trans->SetPosition(i * 3.0f, 0, 0);

        auto meshRenderer = ent.Add<MeshRenderer>();
        meshRenderer->AddModel("assets/Cube.glb");
        //meshRenderer->SetMaterial(0, "assets/test.mat");

        auto bonding = ent.Add<Bounding>();
    }


    for (int i = 1; i < 5; ++i)
    {
        auto ent = mainScene->CreateEntity();

        auto trans = ent.Add<Transform>();
        trans->SetPosition(0, i * 3.0f, 0);

        auto meshRenderer = ent.Add<MeshRenderer>();
        meshRenderer->AddModel("assets/sphere.glb");
        meshRenderer->SetMaterial(0, "assets/test.mat");

        auto bonding = ent.Add<Bounding>();
    }
// 
//     for (int i = 1; i < 3; ++i)
//     {
//         auto ent = mainScene->CreateEntity();
// 
//         auto trans = ent.Add<Transform>();
//         trans->SetPosition(0, i * 3.0f, 0);
// 
//         auto meshRenderer = ent.Add<MeshRenderer>();
//         meshRenderer->AddModel("assets/monkey.glb");
//         meshRenderer->SetMaterial(0, "assets/test.mat"); // default mat
// 
//         auto bonding = ent.Add<Bounding>();
//     }
// 
//     for (int i = 1; i < 3; ++i)
//     {
//         auto ent = mainScene->CreateEntity();
// 
//         auto trans = ent.Add<Transform>();
//         trans->SetPosition(0, -i * 3.0f, 0);
// 
//         auto meshRenderer = ent.Add<MeshRenderer>();
//         meshRenderer->AddModel("assets/cube.glb");
//         meshRenderer->SetMaterial(0, "assets/test.mat"); // default mat
// 
//         auto bonding = ent.Add<Bounding>();
//     }
// 
//     for (int i = 3; i < 5; ++i)
//     {
//         auto ent = mainScene->CreateEntity();
// 
//         auto trans = ent.Add<Transform>();
//         trans->SetPosition(0, -i * 3.0f, 0);
// 
//         auto meshRenderer = ent.Add<MeshRenderer>();
//         meshRenderer->AddModel("assets/sphere.glb");
//         meshRenderer->SetMaterial(0, "assets/test.mat"); // default mat
// 
//         auto bonding = ent.Add<Bounding>();
//     }

    while (engine.GetMainWindow()->ShouldClose())
    {
        engine.Update();
        firstPersonCtrl.Update();
    }

    engine.Shutdown();

    return 0;
}
