#pragma once
#include "core/Common.h"
#include "Scene.h"

namespace rush
{


    class Engine : public Singleton<Engine>
    {
    public:

        Ref<Scene> CreateScene(const StringView& name);

    };

}