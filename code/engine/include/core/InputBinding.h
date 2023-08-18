#pragma once

#include "MathUtils.h"
#include "EcsSystem.h"

namespace rush
{

    class InputBinding : public Component
    {
    public:
        REGISTER_COMPONENT_TYPE(InputBinding)

        InputBinding();
        ~InputBinding();

        int GetMouseX() const { return m_MouseX; }
        int GetMouseY() const { return m_MouseY; }
        int GetMouseZ() const { return m_MouseZ; }

        void AddWindowFilter(const std::string& windowName)
        {
            m_WindowFilters.insert(windowName);
        }

        IVector2 GetMousePos() const { return IVector2(m_MouseX, m_MouseY); }

        bool OnMouseButtonPressed(MouseButtonPressedEvent& event);
        bool OnMouseButtonReleased(MouseButtonReleasedEvent& event);
        bool OnMouseMoveEvent(MouseMovedEvent& event);
        bool OnMouseScrolled(MouseScrolledEvent& event);
        bool OnKeyPressed(KeyPressedEvent& event);
        bool OnKeyReleased(KeyReleasedEvent& event);

        void OnDestroy() override;

        template <typename T>
        void RegisterMousePressed(const std::string& component, std::function<bool(T*)> foo, MouseCode mouseCode)
        {
            InputBinding::BindingMousePressType<T>* binding = new InputBinding::BindingMousePressType<T>;
            binding->Bind();
            binding->component = component;
            binding->foo = foo;
            m_MousePressBindings[mouseCode].push_back(binding);
        }

        template <typename T>
        void RegisterMouseReleased(const std::string& component, std::function<bool(T*)> foo, MouseCode mouseCode)
        {
            InputBinding::BindingMouseReleaseType<T>* binding = new InputBinding::BindingMouseReleaseType<T>;
            binding->Bind();
            binding->component = component;
            binding->foo = foo;
            m_MouseReleaseBindings[mouseCode].push_back(binding);
        }

        template <typename T>
        void RegisterMouseMoved(const std::string& component, std::function<bool(T*, int, int)> foo)
        {
            InputBinding::BindingMouseMoveType<T>* binding = new InputBinding::BindingMouseMoveType<T>;
            binding->Bind();
            binding->component = component;
            binding->foo = foo;
            m_MouseMoveBindings.push_back(binding);
        }

        template <typename T>
        void RegisterMouseScrolled(const std::string& component, std::function<bool(T*, int)> foo)
        {
            InputBinding::BindingMouseScrollType<T>* binding = new InputBinding::BindingMouseScrollType<T>;
            binding->Bind();
            binding->component = component;
            binding->foo = foo;
            m_MouseScrollBindings.push_back(binding);
        }

        template <typename T, typename Key>
        void RegisterKeyPressed(const std::string& component, std::function<bool(T*)> foo, Key keyCode)
        {
            InputBinding::BindingKeyPressType<T>* binding = new InputBinding::BindingKeyPressType<T>;
            binding->Bind();
            binding->component = component;
            binding->foo = foo;
            m_KeyPressBindings[keyCode].push_back(binding);
        }

        template <typename T, typename Key>
        void RegisterKeyReleased(const std::string& component, std::function<bool(T*)> foo, Key keyCode)
        {
            InputBinding::BindingKeyReleaseType<T>* binding = new InputBinding::BindingKeyReleaseType<T>;
            binding->Bind();
            binding->component = component;
            binding->foo = foo;
            m_KeyReleaseBindings[keyCode].push_back(binding);
        }

    protected:
        int m_MouseX = 0;
        int m_MouseY = 0;
        int m_MouseZ = 0;

        std::set<std::string> m_WindowFilters;


        // mouse press
        struct BindingMousePress
        {
            std::string component;
            Delegate<bool(Component*)> execute;
        };

        template <typename T>
        struct BindingMousePressType : public BindingMousePress
        {
            void Bind()
            {
                execute.bind<BindingMousePressType<T>, &BindingMousePressType<T>::executeImpl>(this);
            }

            std::function<bool(T*)> foo;

            bool executeImpl(Component* component)
            {
                return foo(static_cast<T*>(component));
            }
        };

        std::map<MouseCode, std::list<BindingMousePress*>> m_MousePressBindings;

        // mouse release
        struct BindingMouseRelease
        {
            std::string component;
            Delegate<bool(Component*)> execute;
        };

        template <typename T>
        struct BindingMouseReleaseType : public BindingMouseRelease
        {
            void Bind()
            {
                execute.bind<BindingMouseReleaseType<T>, &BindingMouseReleaseType<T>::executeImpl>(this);
            }

            std::function<bool(T*)> foo;

            bool executeImpl(Component* component)
            {
                return foo(static_cast<T*>(component));
            }
        };

        std::map<MouseCode, std::list<BindingMouseRelease*>> m_MouseReleaseBindings;

        // mouse move
        struct BindingMouseMove
        {
            std::string component;
            Delegate<bool(Component*, int x, int y)> execute;
        };

        template <typename T>
        struct BindingMouseMoveType : public BindingMouseMove
        {
            void Bind()
            {
                execute.bind<BindingMouseMoveType<T>, &BindingMouseMoveType<T>::executeImpl>(this);
            }

            std::function<bool(T*, int, int)> foo;

            bool executeImpl(Component* component, int x, int y)
            {
                return foo(static_cast<T*>(component), x, y);
            }
        };

        std::list<BindingMouseMove*> m_MouseMoveBindings;

        // mouse scroll
        struct BindingMouseScroll
        {
            std::string component;
            Delegate<bool(Component*, int)> execute;
        };

        template <typename T>
        struct BindingMouseScrollType : public BindingMouseScroll
        {
            void Bind()
            {
                execute.bind<BindingMouseScrollType<T>, &BindingMouseScrollType<T>::executeImpl>(this);
            }

            std::function<bool(T*, int)> foo;

            bool executeImpl(Component* component, int value)
            {
                return foo(static_cast<T*>(component), value);
            }
        };

        std::list<BindingMouseScroll*> m_MouseScrollBindings;

        // key press
        struct BindingKeyPress
        {
            std::string component;
            Delegate<bool(Component*)> execute;
        };

        template <typename T>
        struct BindingKeyPressType : public BindingKeyPress
        {
            void Bind()
            {
                execute.bind<BindingKeyPressType<T>, &BindingKeyPressType<T>::executeImpl>(this);
            }

            std::function<bool(T*)> foo;

            bool executeImpl(Component* component)
            {
                return foo(static_cast<T*>(component));
            }
        };

        std::map<KeyCode, std::list<BindingKeyPress*>> m_KeyPressBindings;

        // key release
        struct BindingKeyRelease
        {
            std::string component;
            Delegate<bool(Component*)> execute;
        };

        template <typename T>
        struct BindingKeyReleaseType : public BindingKeyRelease
        {
            void Bind()
            {
                execute.bind<BindingKeyReleaseType<T>, &BindingKeyReleaseType<T>::executeImpl>(this);
            }

            std::function<bool(T*)> foo;

            bool executeImpl(Component* component)
            {
                return foo(static_cast<T*>(component));
            }
        };

        std::map<KeyCode, std::list<BindingKeyRelease*>> m_KeyReleaseBindings;
    };

}