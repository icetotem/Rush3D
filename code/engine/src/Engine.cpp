#include "stdafx.h"
#include "Engine.h"
#include "render/RenderContext.h"
#include "version.h"
#include "components/EcsSystem.h"

namespace rush
{

    Engine* Engine::s_Inst = nullptr;

    Engine::Engine()
    {
        s_Inst = this;

        Logger::Init("Rush3D.log", "Rush3D");
        LOG_INFO("Version {}", VERSION);
    }

    void Engine::Init()
    {
        m_RenderManager.Init();

    }

    Ref<Window> Engine::CreateRenderWindow(const WindowDesc& desc, const RendererDesc& rendererDesc)
    {
        auto window = Window::Construct();
        window->Create(desc);
        window->m_Renderer = RenderContext::Construct(window, rendererDesc);
        return window;
    }

    void Engine::Update()
    {
        Timer::Tick();
        m_SceneManager.Update();
        m_RenderManager.Update();
        Entity::ClearRecycle();
    }

    void Engine::Shutdown()
    {
        m_RenderManager.Shutdown();
    }

}