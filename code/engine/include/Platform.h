#pragma once

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
        const wchar_t* title;
        int width;
        int height;
        WindowMode mode;
        bool transparent;
    };

    typedef struct WindowHandleImpl* WindowHandle;
    typedef bool (*Redraw) ();

    class Platform
    {
    public:

        static WindowHandle CreateRenderWindow(const WindowDesc* desc);

        static void DestroyRenderWindow(WindowHandle window);

        static void GetWindowSize(WindowHandle window, int& width, int& height);

        static void StartLoop(WindowHandle window, Redraw redraw);

        static void ShowRenderWindow(WindowHandle window, bool show);
    };


} // namespace rush