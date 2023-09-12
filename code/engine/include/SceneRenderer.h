#ifndef RenderManager_h__
#define RenderManager_h__

#include "core/Common.h"

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
    };

}

#endif // RenderManager_h__


