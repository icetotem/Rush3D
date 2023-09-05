#ifndef CameraCtrl_h__
#define CameraCtrl_h__

#include "components/EcsSystem.h"

namespace rush
{
    
    class CameraCtrlStandard : public Component
    {
    public:
        enum CamCtrlButtons
        {
            CamCtrlForward,
            CamCtrlBackward,
            CamCtrlLeft,
            CamCtrlRight,
            CamCtrlUp,
            CamCtrlDown,
            CamCtrlCount,
        };

        CameraCtrlStandard(Entity owner);

        void OnInit() override;

        void update(float deltaTime, const IVector3& mouseState, bool mouseDown, bool mouseUp, bool keys[CamCtrlCount]);

    protected:
        void reset();
        void getViewMtx(float* _viewMtx);
        void setPosition(const Vector3& _pos);
        void setVerticalAngle(float _verticalAngle);
        void setHorizontalAngle(float _horizontalAngle);

    private:
        struct MouseCoords
        {
            int32_t m_mx;
            int32_t m_my;
            int32_t m_mz;
        };

        MouseCoords m_mouseNow;
        MouseCoords m_mouseLast;

        Vector3 m_eye = Vector3(0);
        Vector3 m_at = Vector3(0);
        Vector3 m_up = Vector3(0);
        float m_horizontalAngle;
        float m_verticalAngle;

        float m_mouseSpeed;
        float m_gamepadSpeed;
        float m_moveSpeed;

        bool m_mouseDown;
    };

}

#endif // CameraCtrl_h__
