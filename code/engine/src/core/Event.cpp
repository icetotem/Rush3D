#include "stdafx.h"
#include "Core/Event.h"

namespace rush
{

    EventManager* EventManager::EventManager::s_Inst = nullptr;

    EventManager::EventManager()
    {
        s_Inst = this;
    }

    EventManager::~EventManager()
    {

    }

    void EventManager::RegisterCallbacks(std::initializer_list<EventType> eventTypes, EventCallbackFn callback)
    {
        for (auto type : eventTypes)
        {
            RegisterCallback(type, callback);
        }
    }

    void EventManager::RegisterCallback(EventType eventType, EventCallbackFn callback)
    {
        const auto& type = callback.target_type();
        std::string name = type.name();
        auto& callbacks = m_EventCallbacks[eventType];
        if (std::find_if(callbacks.begin(), callbacks.end(), [name](const EventCallbackFn& c)->bool{ return name == c.target_type().name(); }) == callbacks.end()) // 排重，防止重复执行消息
        {
            callbacks.push_back(callback);
        }
        else
        {
            LOG_WARN("Duplicate callback function {0}", name.c_str());
        }
    }

    void EventManager::UnRegisterCallbacks(std::initializer_list<EventType> eventTypes, EventCallbackFn callback)
    {
        for (auto type : eventTypes)
        {
            UnRegisterCallback(type, callback);
        }
    }

    void EventManager::UnRegisterCallback(EventType eventType, EventCallbackFn callback)
    {
        const auto& type = callback.target_type();
        std::string name = type.name();
        auto& callbacks = m_EventCallbacks[eventType];
        auto iter = std::find_if(callbacks.begin(), callbacks.end(), [name](const EventCallbackFn& c)->bool { return name == c.target_type().name(); });
        if (iter != callbacks.end())
        {
            callbacks.erase(iter);
        }
        else
        {
            LOG_WARN("Cannot find Event {0}", name.c_str());
        }
    }

    void EventManager::PublishEvent(Event& event)
    {
        auto iter = m_EventCallbacks.find(event.GetType());
        if (iter != m_EventCallbacks.end())
        {
            for (auto callIter : iter->second)
            {
                callIter(event);
            }
        }
    }

}