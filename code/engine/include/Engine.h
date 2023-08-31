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

        void Init();

        Ref<Window> CreateRenderWindow(const WindowDesc& desc, const RendererDesc& rendererDesc);

        void Update();

        void Shutdown();

    private:    
    };

}

#endif // Engine_h__
