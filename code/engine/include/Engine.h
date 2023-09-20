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
        BundleManager bundleManager;
        AssetsManager assetsManager;
        SceneManager sceneManager;

    public:
        Engine();

        void Init();

        Ref<Window> CreateRenderWindow(const WindowDesc& desc);

        Ref<Renderer> CreateRenderer(uint32_t width, uint32_t height);

        void Update();

        void Shutdown();

    private:    
    };

}

#endif // Engine_h__
