#ifndef CameraCtrl_h__
#define CameraCtrl_h__

#include "Window.h"
#include "components/EcsSystem.h"

namespace rush
{
    
    class CameraCtrlFirstPerson
    {
    public:
        CameraCtrlFirstPerson();

        void Setup(Ref<Window> window, Entity camera);

        void Update();

    public:
        float m_MoveSpeed = 5.0f;
        float m_OrbitSpeed = 5.0f;
        float m_ZoomSpeed = 7.0f;

    protected:
        void OnMouseMove(int x, int y);
        void Zoom(int delta);

    private:
        Entity m_Camera;
        bool m_Rotating = false;
        bool m_MovingLeft = false;
        bool m_MovingRight = false;
        bool m_MovingForward = false;
        bool m_MovingBackward = false;
        bool m_MovingUp = false;
        bool m_MovingDown = false;
        float m_Distance = 0;
        int m_LastMouseX = 0;
        int m_LastMouseY = 0;
        bool m_BoostMove = false;
    };

}

#endif // CameraCtrl_h__
