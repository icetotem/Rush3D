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
    engine.Init(wndDesc);

    engine.GetBundleManager().AddFolder("../../assets");

    auto mainScene = engine.GetSceneManager().GetMainScene();
    auto mainCamera = mainScene->GetMainCamera();

    CameraCtrlFirstPerson firstPersonCtrl;
    firstPersonCtrl.Setup(engine.GetMainWindow(), mainCamera);

    // create the entity
    {
        auto ent0 = mainScene->CreateEntity("ent0");

        auto trans = ent0.Add<Transform>();
        trans->SetPosition(0, 0, 0);

        auto meshRenderer = ent0.Add<MeshRenderer>();
        meshRenderer->AddMesh("assets/monkey.glb");

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
