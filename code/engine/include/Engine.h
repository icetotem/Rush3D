#ifndef Engine_h__
#define Engine_h__

#include "core/Common.h"
#include "render/Renderer.h"
#include "Window.h"
#include "SceneManager.h"
#include "AssetManager.h"
#include "BundleManager.h"

namespace rush
{

    class Engine : public Singleton<Engine>
    {
    public:
        Engine();

        void Init(const WindowDesc& mainWndDesc);

        Ref<Window> CreateRenderWindow(const WindowDesc& desc);

        Ref<Renderer> CreateRenderer(uint32_t width, uint32_t height);

        void Update();

        void Shutdown();

        Ref<Window> GetMainWindow() const { return m_MainWindow; }

        Ref<Renderer> GetMainRenderer() const { return m_MainRenderer; }

        BundleManager& GetBundleManager() { return m_BundleManager; }

        AssetsManager& GetAssetsManager() { return m_AssetsManager; }

        SceneManager& GetSceneManager() { return m_SceneManager; }

    private:    
        Ref<Window> m_MainWindow;
        Ref<Renderer> m_MainRenderer;
        BundleManager m_BundleManager;
        AssetsManager m_AssetsManager;
        SceneManager m_SceneManager;
    };

}

#endif // Engine_h__
