#ifndef Window_h__
#define Window_h__

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
        bool alwaysTop = true;
        bool visible = true;
    };

    typedef void* WindowHandle;
    typedef void* DisplayHandle;

    class Renderer;

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

        Ref<Renderer> GetRenderer() const { return m_Renderer; }

        bool ShouldClose() const;
        void MessgeLoop();

        void Show(bool show = true);

        void Present();

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
        Ref<Renderer> m_Renderer;

        String m_Title;
        uint32_t m_Width = 1024;
        uint32_t m_Height = 768;

        WindowHandle m_Handle = nullptr;
        DisplayHandle m_Display = nullptr;
    };

}

#endif // Window_h__
