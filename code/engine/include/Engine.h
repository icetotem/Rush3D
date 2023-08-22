#pragma once
#include "core/Common.h"
#include "Window.h"
#include "Scene.h"

namespace rush
{


    class Engine : public Singleton<Engine>
    {
    public:
        Engine();

        Ref<Window> CreateRenderWindow(const WindowDesc& desc);

        Ref<Scene> CreateScene(const StringView& name);

    };

}