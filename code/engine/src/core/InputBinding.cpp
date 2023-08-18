#include "stdafx.h"
#include "Core/InputBinding.h"

namespace rush
{

    REGISTER_COMPONENT_FACTORY(InputBinding)

    InputBinding::InputBinding()
    {

    }

    InputBinding::~InputBinding()
    {

    }

    bool InputBinding::OnMouseButtonPressed(MouseButtonPressedEvent& event)
    {
        if (m_WindowFilters.size() > 0 && m_WindowFilters.find(event.GetWindowID()) == m_WindowFilters.end())
        {
            return false;
        }

        auto iter = m_MousePressBindings.find(event.GetMouseButton());
        if (iter != m_MousePressBindings.end())
        {
            for (auto binding : iter->second)
            {
                auto component = GetOwner()->GetComponentByName(binding->component);
                if (component && binding->execute.invoke(component))
                    return true;
            }
        }

        return false;
    }

    bool InputBinding::OnMouseButtonReleased(MouseButtonReleasedEvent& event)
    {
        if (m_WindowFilters.size() > 0 && m_WindowFilters.find(event.GetWindowID()) == m_WindowFilters.end())
        {
            return false;
        }

        auto iter = m_MouseReleaseBindings.find(event.GetMouseButton());
        if (iter != m_MouseReleaseBindings.end())
        {
            for (auto binding : iter->second)
            {
                auto component = GetOwner()->GetComponentByName(binding->component);
                if (component && binding->execute.invoke(component))
                    return true;
            }
        }
        return false;
    }

    bool InputBinding::OnMouseMoveEvent(MouseMovedEvent& event)
    {
        if (m_WindowFilters.size() > 0 && m_WindowFilters.find(event.GetWindowID()) == m_WindowFilters.end())
        {
            return false;
        }

        m_MouseX = event.GetX();
        m_MouseY = event.GetY();

        for (auto& binding : m_MouseMoveBindings)
        {
            auto component = GetOwner()->GetComponentByName(binding->component);
            if (component)
            {
                if (binding->execute.invoke(component, event.GetX(), event.GetY()))
                    return true;
            }
        }
        return false;
    }

    bool InputBinding::OnMouseScrolled(MouseScrolledEvent& event)
    {
        if (m_WindowFilters.size() > 0 && m_WindowFilters.find(event.GetWindowID()) == m_WindowFilters.end())
        {
            return false;
        }

        for (auto& binding : m_MouseScrollBindings)
        {
            auto component = GetOwner()->GetComponentByName(binding->component);
            if (component)
            {
                if (binding->execute.invoke(component, event.GetYOffset()))
                    return true;
            }
        }
        return false;
    }

    bool InputBinding::OnKeyPressed(KeyPressedEvent& event)
    {
        if (m_WindowFilters.size() > 0 && m_WindowFilters.find(event.GetWindowID()) == m_WindowFilters.end())
        {
            return false;
        }

        auto iter = m_KeyPressBindings.find(event.GetKeyCode());
        if (iter != m_KeyPressBindings.end())
        {
            for (auto binding : iter->second)
            {
                auto component = GetOwner()->GetComponentByName(binding->component);
                if (component && binding->execute.invoke(component))
                    return true;
            }            
        }

        return false;
    }

    bool InputBinding::OnKeyReleased(KeyReleasedEvent& event)
    {
        if (m_WindowFilters.size() > 0 && m_WindowFilters.find(event.GetWindowID()) == m_WindowFilters.end())
        {
            return false;
        }

        auto iter = m_KeyReleaseBindings.find(event.GetKeyCode());
        if (iter != m_KeyReleaseBindings.end())
        {
            for (auto binding : iter->second)
            {
                auto component = GetOwner()->GetComponentByName(binding->component);
                if (component && binding->execute.invoke(component))
                    return true;
            }
        }

        return false;
    }

    void InputBinding::OnDestroy()
    {
        for (auto pair : m_MousePressBindings)
        {
            for (auto binding : pair.second)
            {
                delete binding;
            }
        }
        m_MousePressBindings.clear();

        for (auto pair : m_MouseReleaseBindings)
        {
            for (auto binding : pair.second)
            {
                delete binding;
            }
        }
        m_MouseReleaseBindings.clear();

        for (auto binding : m_MouseMoveBindings)
        {
            delete binding;
        }
        m_MouseMoveBindings.clear();

        for (auto binding : m_MouseScrollBindings)
        {
            delete binding;
        }
        m_MouseScrollBindings.clear();

        for (auto pair : m_KeyPressBindings)
        {
            for (auto binding : pair.second)
            {
                delete binding;
            }
        }
        m_KeyPressBindings.clear();

        for (auto pair : m_KeyReleaseBindings)
        {
            for (auto binding : pair.second)
            {
                delete binding;
            }
        }
        m_KeyReleaseBindings.clear();
    }

}