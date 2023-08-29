#include "stdafx.h"
#include "Engine.h"
#include "render/Renderer.h"
#include "version.h"

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

    Ref<Scene> Engine::CreateScene(const StringView& name)
    {
        auto scene = CreateRef<Scene>();
        m_Scenes.insert({name, scene});
        return scene;
    }

    void Engine::Update()
    {
        
    }

}