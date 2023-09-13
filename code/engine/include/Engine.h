#ifndef Engine_h__
#define Engine_h__

#include "core/Common.h"
#include "render/RenderContext.h"
#include "Window.h"
#include "SceneManager.h"
#include "SceneRenderer.h"
#include "AssetManager.h"
#include "BundleManager.h"

namespace rush
{

    class Engine : public Singleton<Engine>
    {
    public:
        BundleManager m_BundleManager;
        AssetsManager m_AssetsManager;
        SceneManager m_SceneManager;
        SceneRenderer m_RenderManager;

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
