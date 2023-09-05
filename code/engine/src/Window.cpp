#include "stdafx.h"
#include "Window.h"
#include "render/Renderer.h"

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

    //////////////////////////////////////////////////////////////////////////

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

        // input callbacks
        glfwSetWindowUserPointer(m_Impl->handle, (void*)this);

        glfwSetWindowSizeCallback(m_Impl->handle, [](GLFWwindow* window, int width, int height)
        {
            Window* rd = (Window*)glfwGetWindowUserPointer(window);

        });

        glfwSetWindowCloseCallback(m_Impl->handle, [](GLFWwindow* window)
        {
            Window* rd = (Window*)glfwGetWindowUserPointer(window);

        });

        glfwSetWindowFocusCallback(m_Impl->handle, [](GLFWwindow* window, int focus)
        {
            Window* rd = (Window*)glfwGetWindowUserPointer(window);
        });

        glfwSetKeyCallback(m_Impl->handle, [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            Window* rd = (Window*)glfwGetWindowUserPointer(window);
            switch (action)
            {
            case GLFW_PRESS:
            {
                for (auto iter : rd->m_KeyCallbacks)
                {
                    std::function<bool(InputButtonState, KeyCode)> function = iter;
                    function(InputButtonState::Pressed, KeyCode(key));
                }
                break;
            }
            case GLFW_RELEASE:
            {
                for (auto iter : rd->m_KeyCallbacks)
                {
                    std::function<bool(InputButtonState, KeyCode)> function = iter;
                    function(InputButtonState::Released, KeyCode(key));
                }
                break;
            }
            case GLFW_REPEAT:
            {
                for (auto iter : rd->m_KeyCallbacks)
                {
                    std::function<bool(InputButtonState, KeyCode)> function = iter;
                    function(InputButtonState::Repeat, KeyCode(key));
                }
                break;
            }
            }
        });

        glfwSetCharCallback(m_Impl->handle, [](GLFWwindow* window, unsigned int keycode)
        {
            Window* rd = (Window*)glfwGetWindowUserPointer(window);
            for (auto iter : rd->m_CharCallbacks)
            {
                std::function<bool(uint16_t)> function = iter;
                function(keycode);
            }
        });

        glfwSetMouseButtonCallback(m_Impl->handle, [](GLFWwindow* window, int button, int action, int mods)
        {
            Window* rd = (Window*)glfwGetWindowUserPointer(window);
            switch (action)
            {
            case GLFW_PRESS:
            {
                for (auto iter : rd->m_MouseButtonCallbacks)
                {
                    std::function<bool(InputButtonState, MouseCode, uint32_t mouseX, uint32_t mouseY)> function = iter;
                    function(InputButtonState::Pressed, MouseCode(button), rd->m_MouseX, rd->m_MouseY);
                }
                break;
            }
            case GLFW_RELEASE:
            {
                for (auto iter : rd->m_MouseButtonCallbacks)
                {
                    std::function<bool(InputButtonState, MouseCode, uint32_t mouseX, uint32_t mouseY)> function = iter;
                    function(InputButtonState::Released, MouseCode(button), rd->m_MouseX, rd->m_MouseY);
                }
                break;
            }
            }
        });

        glfwSetScrollCallback(m_Impl->handle, [](GLFWwindow* window, double xOffset, double yOffset)
        {
            Window* rd = (Window*)glfwGetWindowUserPointer(window);
            for (auto iter : rd->m_MouseWheelCallbacks)
            {
                std::function<bool(uint32_t, uint32_t)> function = iter;
                function((uint32_t)xOffset, (uint32_t)yOffset);
            }
        });

        glfwSetCursorPosCallback(m_Impl->handle, [](GLFWwindow* window, double xPos, double yPos)
        {
            Window* rd = (Window*)glfwGetWindowUserPointer(window);
            for (auto iter : rd->m_MouseMoveCallbacks)
            {
                std::function<bool(uint32_t, uint32_t)> function = iter;
                function((uint32_t)xPos, (uint32_t)yPos);
            }
        });

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

    void Window::Close()
    {
        glfwDestroyWindow(m_Impl->handle);
    }

    void Window::Present()
    {
        m_Renderer->Present();
    }

    void Window::BindKeyCallback(std::function<bool(InputButtonState, KeyCode)> callback)
    {
        m_KeyCallbacks.push_back(callback);
    }

    void Window::BindCharCallback(std::function<bool(uint16_t)> callback)
    {
        m_CharCallbacks.push_back(callback);    
    }

    void Window::BindMouseButtonCallback(std::function<bool(InputButtonState, MouseCode, uint32_t mouseX, uint32_t mouseY)> callback)
    {
        m_MouseButtonCallbacks.push_back(callback);
    }

    void Window::BindMouseMoveCallback(std::function<bool(uint32_t mouseX, uint32_t mouseY)> callback)
    {
        m_MouseMoveCallbacks.push_back(callback);
    }

    void Window::BindMouseWheelCallback(std::function<bool(uint32_t deltaH, uint32_t deltaV)> callback)
    {
        m_MouseWheelCallbacks.push_back(callback);
    }

}

#else // defined(RUSH_PLATFORM_WINDOWS) || defined(RUSH_PLATFORM_MAC) || defined(RUSH_PLATFORM_LINUX)
#error not yet implemented
#endif // platform defines