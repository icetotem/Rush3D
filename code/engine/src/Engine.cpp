#include "stdafx.h"
#include "Engine.h"
#include "render/Renderer.h"
#include "version.h"
#include "components/EcsSystem.h"

namespace rush
{
    Engine* Engine::s_Inst = nullptr;

    Engine::Engine()
    {
        Logger::Init("Rush3D.log", "Rush3D");
        LOG_INFO("Version {}", VERSION);
    }

    Ref<Window> Engine::CreateRenderWindow(const WindowDesc& desc)
    {
        auto window = Window::Construct();
        window->Create(desc);
        return window;
    }

    Ref<Renderer> Engine::CreateRenderer(Ref<Window> window, const RendererDesc* rendererDesc)
    {
        auto renderer = Renderer::Construct(window, rendererDesc);
        return renderer;
    }

    void Engine::Update()
    {
        m_SceneManager.Update();
        m_RenderManager.Update();

        Entity::ClearRecycle();
    }

}