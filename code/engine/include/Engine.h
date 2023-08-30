#ifndef Engine_h__
#define Engine_h__

#include "core/Common.h"
#include "render/Renderer.h"
#include "Window.h"
#include "Scene.h"
#include "RenderManager.h"

namespace rush
{

    class Engine : public Singleton<Engine>
    {
    public:
        SceneManager m_SceneManager;
        RenderManager m_RenderManager;

    public:
        Engine();

        Ref<Window> CreateRenderWindow(const WindowDesc& desc);

        Ref<Renderer> CreateRenderer(Ref<Window> window, const RendererDesc* rendererDesc);

        void Update();

    private:    
    };

}

#endif // Engine_h__
