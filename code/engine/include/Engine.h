#pragma once
#include "core/Common.h"
#include "Window.h"
#include "render/Renderer.h"
#include "Scene.h"

namespace rush
{
    
    class Window;
    class Renderer;
    class Scene;
    struct WindowDesc;
    struct RendererDesc;
    struct RenderPassDesc;

    class Engine : public Singleton<Engine>
    {
    public:
        Engine();

        Ref<Window> CreateRenderWindow(const WindowDesc& desc);

        Ref<Renderer> CreateRenderer(Ref<Window> window, const RendererDesc* rendererDesc);

        Ref<Scene> CreateScene(const StringView& name);

    };

}