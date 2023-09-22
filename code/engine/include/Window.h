#ifndef Window_h__
#define Window_h__

#include "InputDefne.h"
#include "render/RSurface.h"
#include "render/Renderer.h"

namespace rush
{

    enum class WindowMode
    {
        Windowed,
        Maximized,
        Fullscreen,
    };

    struct WindowDesc
    {
        String title = "Rush3D";
        uint32_t width = 1024;
        uint32_t height = 768;
        int posX = -1;
        int posY = -1;
        WindowMode windowMode = WindowMode::Windowed;
        bool transparent = false;
        bool frameless = false;
        bool resizable = true;
        bool alwaysTop = false;
        bool visible = true;
        bool vsync = true;
        void* parentHwnd = nullptr;
    };

    typedef void* WindowHandle;
    typedef void* DisplayHandle;

    class Window
    {
    public:
        ~Window();

    public:
        WindowHandle GetNativeHandle() const { return m_Handle; }
        DisplayHandle GetDisplay() const { return m_Display; }

        const String& GetTitle() const { return m_Title; }
        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }

        uint32_t GetMouseX() const { return m_MouseX; }
        uint32_t GetMouseY() const { return m_MouseY; }

        Ref<RSurface> GetSurface() { return m_Surface; }

        bool ShouldClose() const;
        static void MessgeLoop();

        void Show(bool show = true);

        void Close();

        void Present();

        void BindKeyCallback(std::function<bool(InputButtonState, KeyCode)> callback);
        void BindCharCallback(std::function<bool(uint16_t)> callback);
        void BindMouseButtonCallback(std::function<bool(InputButtonState, MouseCode, uint32_t mouseX, uint32_t mouseY)> callback);
        void BindMouseMoveCallback(std::function<bool(uint32_t mouseX, uint32_t mouseY)> callback);
        void BindMouseWheelCallback(std::function<bool(uint32_t delta)> callback);

    protected:
        friend class Engine;

        Window();

        static Ref<Window> Construct()
        {
            return std::shared_ptr<Window>(new Window());
        }

        bool Create(const WindowDesc& desc);

    protected:
        struct Impl;
        Ref<Impl> m_Impl;
        Ref<RSurface> m_Surface;

        String m_Title;
        uint32_t m_Width = 1024;
        uint32_t m_Height = 768;

        WindowHandle m_Handle = nullptr;
        DisplayHandle m_Display = nullptr;

        List<std::function<bool(InputButtonState, KeyCode)>> m_KeyCallbacks;
        List<std::function<bool(uint16_t)>> m_CharCallbacks;
        List<std::function<bool(InputButtonState, MouseCode, uint32_t mouseX, uint32_t mouseY)>> m_MouseButtonCallbacks;
        List<std::function<bool(uint32_t mouseX, uint32_t mouseY)>> m_MouseMoveCallbacks;
        List<std::function<bool(uint32_t delta)>> m_MouseWheelCallbacks;

        uint32_t m_MouseX = 0;
        uint32_t m_MouseY = 0;
    };

}

#endif // Window_h__
