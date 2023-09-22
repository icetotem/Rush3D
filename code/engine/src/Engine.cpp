#include "stdafx.h"
#include "Engine.h"
#include "render/Renderer.h"
#include "version.h"
#include "components/EcsSystem.h"
#include "render/RDevice.h"

namespace rush
{

    Engine* Engine::s_Inst = nullptr;

    Engine::Engine()
    {
        s_Inst = this;

        Logger::Init("Rush3D.log", "Rush3D");
        LOG_INFO("Version {}", VERSION);

        static RDevice device;
        device.Create(wgpu::BackendType::D3D12);
    }

    void Engine::Init(const WindowDesc& mainWndDesc)
    {
        m_MainWindow = CreateRenderWindow(mainWndDesc);
        m_MainRenderer = CreateRenderer(mainWndDesc.width, mainWndDesc.height);
    }

    Ref<Window> Engine::CreateRenderWindow(const WindowDesc& desc)
    {
        auto window = Window::Construct();
        window->Create(desc);
        return window;
    }

    Ref<Renderer> Engine::CreateRenderer(uint32_t width, uint32_t height)
    {
        return Renderer::Construct(width, height);
    }

    void Engine::Update()
    {
        Timer::Tick();
        Window::MessgeLoop();
        m_SceneManager.Update<MainSceneTag>(m_MainRenderer, m_MainWindow->GetSurface());
        Entity::ClearRecycle();
        m_MainWindow->Present();
    }

    void Engine::Shutdown()
    {

    }

}