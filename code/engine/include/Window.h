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
        String Title = "Rush3D";
        uint32_t Width = 1024;
        uint32_t Height = 768;
        int PosX = -1;
        int PosY = -1;
        WindowMode Mode = WindowMode::Windowed;
        bool Transparent = false;
        bool Frameless = false;
        bool Resizable = true;
        bool OnTop = true;
        bool Visible = true;
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

        bool ShouldClose() const;
        void MessgeLoop();

        void Show(bool show = true);

    protected:
        friend class Engine;

        Window();

        static Ref<Window> Construct()
        {
            return std::shared_ptr<Window>(new Window);
        }

        bool Create(const WindowDesc& desc);

    protected:
        struct Impl;
        Ref<Impl> m_Impl;

        String m_Title;
        uint32_t m_Width = 1024;
        uint32_t m_Height = 768;

        WindowHandle m_Handle = nullptr;
        DisplayHandle m_Display = nullptr;
    };

}

#endif // Window_h__
