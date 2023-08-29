#include "stdafx.h"
#include "Window.h"

#if defined(RUSH_PLATFORM_WINDOWS) || defined(RUSH_PLATFORM_MAC) || defined(RUSH_PLATFORM_LINUX) // GLFW implementation

#if defined(RUSH_PLATFORM_WINDOWS)
#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(RUSH_PLATFORM_MAC)
#define GLFW_EXPOSE_NATIVE_COCOA
#elif defined(RUSH_PLATFORM_LINUX)
#define GLFW_EXPOSE_NATIVE_X11
#endif

#include <glfw/glfw3.h>
#include <glfw/include/GLFW/glfw3native.h>

namespace rush
{

    void PrintGLFWError(int code, const char* message)
    {
        LOG_ERROR("GLFW error: {} - {}", code, message);
    }

    static class GlfwInitialzer
    {
    public:
        GlfwInitialzer()
        {
            glfwSetErrorCallback(PrintGLFWError);
            glfwInit();
        }
        ~GlfwInitialzer()
        {
            glfwTerminate();
        }
    } g_GlfwInitialzer;

    struct Window::Impl
    {
        GLFWwindow* handle = nullptr;
    };

    Window::Window()
    {
        m_Impl = CreateRef<Impl>();
    }

    Window::~Window()
    {
        if (m_Impl->handle)
        {
            glfwDestroyWindow(m_Impl->handle);
        }
    }

    static void* GetNativeWindowHandle(GLFWwindow* window)
    {
#ifdef RUSH_PLATFORM_WINDOWS
        return (void*)glfwGetWin32Window(window);
#elif defined(RUSH_PLATFORM_MAC)
        return (void*)glfwGetCocoaWindow(window);
#elif defined(RUSH_PLATFORM_LINUX)
        return (void*)glfwGetX11Window(window);
#else   
        RUSH_ASSERT(false);
        return nullptr;
#endif
    }

    static void* GetDisplay()
    {
#ifdef RUSH_PLATFORM_WINDOWS
        return nullptr;
#elif defined(RUSH_PLATFORM_MAC)
        return (void*)glfwGetX11Display();
#elif defined(RUSH_PLATFORM_LINUX)
        return (void*)glfwGetX11Display();
#else   
        RUSH_ASSERT(false);
        return nullptr;
#endif
    }


    bool Window::Create(const WindowDesc& desc)
    {        
        m_Width = desc.width;
        m_Height = desc.height;
        m_Title = desc.title;

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, desc.resizable ? GLFW_TRUE : GLFW_FALSE);
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, desc.transparent ? GLFW_TRUE : GLFW_FALSE);
        glfwWindowHint(GLFW_DECORATED, desc.frameless ? GLFW_FALSE : GLFW_TRUE);
        glfwWindowHint(GLFW_FLOATING, desc.alwaysTop ? GLFW_TRUE : GLFW_FALSE);
        glfwWindowHint(GLFW_VISIBLE, desc.visible ? GLFW_TRUE : GLFW_FALSE);

        if (desc.posX >= 0)
        {
            glfwWindowHint(GLFW_POSITION_X, desc.posX);
        }

        if (desc.posY >= 0)
        {
            glfwWindowHint(GLFW_POSITION_Y, desc.posY);
        }

        if (desc.windowMode == WindowMode::Maximized)
        {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        }

        m_Impl->handle = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);
        if (m_Impl->handle == nullptr)
        {
            LOG_ERROR("glfw create window");
            return false;
        }

        m_Handle = GetNativeWindowHandle(m_Impl->handle);
        m_Display = GetDisplay();

        return true;
    }

    bool Window::ShouldClose() const
    {
        return !glfwWindowShouldClose(m_Impl->handle);
    }

    void Window::MessgeLoop()
    {
        glfwPollEvents();
    }

    void Window::Show(bool show /*= true*/)
    {
        if (show)
        {
            glfwShowWindow(m_Impl->handle);
        }
        else
        {
            glfwHideWindow(m_Impl->handle);
        }
    }

}

#else // defined(RUSH_PLATFORM_WINDOWS) || defined(RUSH_PLATFORM_MAC) || defined(RUSH_PLATFORM_LINUX)
#error not yet implemented
#endif // platform defines