#ifndef Engine_h__
#define Engine_h__

#include "core/Common.h"
#include "render/Renderer.h"
#include "Window.h"
#include "Scene.h"

namespace rush
{

    class Engine : public Singleton<Engine>
    {
    public:
        Engine();

        Ref<Window> CreateRenderWindow(const WindowDesc& desc);

        Ref<Renderer> CreateRenderer(Ref<Window> window, const RendererDesc* rendererDesc);

        Ref<Scene> CreateScene(const StringView& name);

    };

}

#endif // Engine_h__
