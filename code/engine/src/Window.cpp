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
#include "render/RDevice.h"
#include "imgui.h"

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

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Do not use OpenGL
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

        m_Impl->handle = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);
        if (m_Impl->handle == nullptr)
        {
            LOG_ERROR("glfw create window");
            return false;
        }

        m_Handle = GetNativeWindowHandle(m_Impl->handle);
        m_Display = GetDisplay();

        m_Surface = RSurface::Construct(m_Width, m_Height, desc.vsync, m_Handle, m_Display);

        if (desc.windowMode == WindowMode::Maximized)
        {
            glfwMaximizeWindow(m_Impl->handle);
        }
        else if (desc.windowMode == WindowMode::Fullscreen)
        {
            // 获取屏幕的宽度和高度
            const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
            int screenWidth = mode->width;
            int screenHeight = mode->height;

            // 切换到全屏模式
            glfwSetWindowMonitor(m_Impl->handle, glfwGetPrimaryMonitor(), 0, 0, screenWidth, screenHeight, GLFW_DONT_CARE);
        }

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
            ImGuiIO& io = ImGui::GetIO();

//             if (key >= 0 && key < IM_ARRAYSIZE(io.KeysDown))
//             {
//                 if (action == GLFW_PRESS)
//                 {
//                     io.KeysDown[key] = true;
//                 }
//                 else if (action == GLFW_RELEASE)
//                 {
//                     io.KeysDown[key] = false;
//                 }
//             }
// 
//             io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
//             io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
//             io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
//             io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];

            if (!io.WantCaptureKeyboard)
            {

                Window* rd = (Window*)glfwGetWindowUserPointer(window);
                switch (action)
                {
                case GLFW_PRESS:
                {
                    for (auto iter : rd->m_KeyCallbacks)
                    {
                        std::function<bool(InputButtonState, KeyCode)> function = iter;
                        if (!function(InputButtonState::Pressed, KeyCode(key)))
                        {
                            break;
                        }
                    }
                    break;
                }
                case GLFW_RELEASE:
                {
                    for (auto iter : rd->m_KeyCallbacks)
                    {
                        std::function<bool(InputButtonState, KeyCode)> function = iter;
                        if (!function(InputButtonState::Released, KeyCode(key)))
                        {
                            break;
                        }
                    }
                    break;
                }
                case GLFW_REPEAT:
                {
                    for (auto iter : rd->m_KeyCallbacks)
                    {
                        std::function<bool(InputButtonState, KeyCode)> function = iter;
                        if (!function(InputButtonState::Repeat, KeyCode(key)))
                        {
                            break;
                        }
                    }
                    break;
                }
                }
            }
        });

        glfwSetCharCallback(m_Impl->handle, [](GLFWwindow* window, unsigned int keycode)
        {
            ImGuiIO& io = ImGui::GetIO();
            io.AddInputCharacter(keycode);

            Window* rd = (Window*)glfwGetWindowUserPointer(window);
            for (auto iter : rd->m_CharCallbacks)
            {
                std::function<bool(uint16_t)> function = iter;
                if (!function(keycode))
                {
                    break;
                }
            }
        });

        glfwSetMouseButtonCallback(m_Impl->handle, [](GLFWwindow* window, int button, int action, int mods)
        {
            ImGuiIO& io = ImGui::GetIO();

            if (button >= 0 && button < IM_ARRAYSIZE(io.MouseDown))
            {
                if (action == GLFW_PRESS)
                {
                    io.MouseDown[button] = true;
                }
                else if (action == GLFW_RELEASE)
                {
                    io.MouseDown[button] = false;
                }

                if (!io.WantCaptureMouse)
                {
                    Window* rd = (Window*)glfwGetWindowUserPointer(window);
                    switch (action)
                    {
                    case GLFW_PRESS:
                    {
                        glfwFocusWindow(window); // focus window when click
                        for (auto iter : rd->m_MouseButtonCallbacks)
                        {
                            std::function<bool(InputButtonState, MouseCode, uint32_t mouseX, uint32_t mouseY)> function = iter;
                            if (!function(InputButtonState::Pressed, MouseCode(button), rd->m_MouseX, rd->m_MouseY))
                            {
                                break;
                            }
                        }
                        break;
                    }
                    case GLFW_RELEASE:
                    {
                        for (auto iter : rd->m_MouseButtonCallbacks)
                        {
                            std::function<bool(InputButtonState, MouseCode, uint32_t mouseX, uint32_t mouseY)> function = iter;
                            if (!function(InputButtonState::Released, MouseCode(button), rd->m_MouseX, rd->m_MouseY))
                            {
                                break;
                            }
                        }
                        break;
                    }
                    }
                }
            }
        });

        glfwSetScrollCallback(m_Impl->handle, [](GLFWwindow* window, double xOffset, double yOffset)
        {
            ImGuiIO& io = ImGui::GetIO();
            io.MouseWheelH += (float)xOffset;
            io.MouseWheel += (float)yOffset;
            if (!io.WantCaptureMouse)
            {
                Window* rd = (Window*)glfwGetWindowUserPointer(window);
                for (auto iter : rd->m_MouseWheelCallbacks)
                {
                    std::function<bool(uint32_t)> function = iter;
                    if (!function((uint32_t)yOffset))
                    {
                        break;
                    }
                }
            }
        });

        glfwSetCursorPosCallback(m_Impl->handle, [](GLFWwindow* window, double xPos, double yPos)
        {
            ImGuiIO& io = ImGui::GetIO();
            io.MousePos.x = (float)xPos;
            io.MousePos.y = (float)yPos;
            if (!io.WantCaptureMouse)
            {
                Window* rd = (Window*)glfwGetWindowUserPointer(window);
                for (auto iter : rd->m_MouseMoveCallbacks)
                {
                    std::function<bool(uint32_t, uint32_t)> function = iter;
                    if (!function((uint32_t)xPos, (uint32_t)yPos))
                    {
                        break;
                    }
                }
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
        m_Surface->Present();
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

    void Window::BindMouseWheelCallback(std::function<bool(uint32_t delta)> callback)
    {
        m_MouseWheelCallbacks.push_back(callback);
    }

}

#else // defined(RUSH_PLATFORM_WINDOWS) || defined(RUSH_PLATFORM_MAC) || defined(RUSH_PLATFORM_LINUX)
#error not yet implemented
#endif // platform defines