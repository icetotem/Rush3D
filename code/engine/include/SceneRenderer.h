#ifndef RenderManager_h__
#define RenderManager_h__

#include "core/Common.h"
#include "render/passes/TransientResources.h"

namespace rush
{

    class SceneRenderer
    {
    public:
        SceneRenderer();
        ~SceneRenderer();

        void Init();

        void Update();

        void Shutdown();

    private:
        TransientResources m_transientResources;
    };

}

#endif // RenderManager_h__


