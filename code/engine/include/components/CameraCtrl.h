#ifndef CameraCtrl_h__
#define CameraCtrl_h__

#include "components/EcsSystem.h"
#include "gainput/gainput.h"
#include "gainput/GainputInputMap.h"

namespace rush
{
    
    class CameraCtrl : public Component
    {
    public:
        CameraCtrl(Entity owner);

        void Update();

    private:
        Ref<gainput::InputMap> m_InputBinding;
    };

}

#endif // CameraCtrl_h__
