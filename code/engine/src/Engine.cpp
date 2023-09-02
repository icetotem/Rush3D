#include "stdafx.h"
#include "Engine.h"
#include "render/Renderer.h"
#include "version.h"
#include "components/EcsSystem.h"

#include "gainput/GainputInputManager.h"
#include "gainput/GainputInputDeviceKeyboard.h"
#include "gainput/GainputInputDeviceMouse.h"
#include "gainput/GainputInputDevicePad.h"
#include "gainput/GainputInputDeviceTouch.h"

namespace rush
{

    Engine* Engine::s_Inst = nullptr;

    Engine::Engine()
    {
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
        window->m_Renderer = Renderer::Construct(window, rendererDesc);


        m_InputManager.SetDisplaySize(desc.width, desc.height);
        keyboardId = m_InputManager.CreateDevice<gainput::InputDeviceKeyboard>();
        mouseId = m_InputManager.CreateDevice<gainput::InputDeviceMouse>();
        padId = m_InputManager.CreateDevice<gainput::InputDevicePad>();
        touchId = m_InputManager.CreateDevice<gainput::InputDeviceTouch>();


        return window;
    }

    void Engine::Update()
    {
#ifdef RUSH_PLATFORM_WINDOWS
        MSG msg;
        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            m_InputManager.HandleMessage(msg);
        }
#endif
        m_InputManager.Update();

        m_SceneManager.Update();
        m_RenderManager.Update();

        Entity::ClearRecycle();
    }

    void Engine::Shutdown()
    {
        m_RenderManager.Shutdown();
    }

}