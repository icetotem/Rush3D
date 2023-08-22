#include "stdafx.h"
#include "Engine.h"

namespace rush
{
    Engine* Engine::s_Inst = nullptr;

    Engine::Engine()
    {
        Logger::Init("Rush3D.log", "Rush3D");
        LOG_INFO("Rush3D Version {}", "0.01.0000");
    }

    Ref<Window> Engine::CreateRenderWindow(const WindowDesc& desc)
    {
        auto window = Window::Construct();
        window->Create(desc);
        return window;
    }

}