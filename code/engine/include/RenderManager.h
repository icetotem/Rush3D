#ifndef RenderManager_h__
#define RenderManager_h__

#include "core/Common.h"

namespace rush
{

    class RenderManager
    {
    public:
        RenderManager();
        ~RenderManager();

        void Init();

        void Update();

        void Shutdown();
    };

}

#endif // RenderManager_h__


