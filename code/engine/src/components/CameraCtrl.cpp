#include "stdafx.h"
#include "components/CameraCtrl.h"
#include "Engine.h"
#include "components/Camera.h"
#include "components/Transform.h"

namespace rush
{

    CameraCtrlStandard::CameraCtrlStandard(Entity owner)
        : Component(owner)
    {
    }

    void CameraCtrlStandard::OnInit()
    {
        reset();
        auto transform = Get<Transform>();
        setPosition(transform->GetPosition());
    }

    void CameraCtrlStandard::reset()
    {
        m_mouseNow.m_mx = 0;
        m_mouseNow.m_my = 0;
        m_mouseNow.m_mz = 0;
        m_mouseLast.m_mx = 0;
        m_mouseLast.m_my = 0;
        m_mouseLast.m_mz = 0;
        m_eye.x = 0.0f;
        m_eye.y = 0.0f;
        m_eye.z = -35.0f;
        m_at.x = 0.0f;
        m_at.y = 0.0f;
        m_at.z = -1.0f;
        m_up.x = 0.0f;
        m_up.y = 1.0f;
        m_up.z = 0.0f;
        m_horizontalAngle = 0.01f;
        m_verticalAngle = 0.0f;
        m_mouseSpeed = 0.0020f;
        m_gamepadSpeed = 0.04f;
        m_moveSpeed = 30.0f;
        m_mouseDown = false;
    }

    static Vector3 mad(Vector3 _a, float _b, Vector3 _c)
    {
        return _a * _b + _c;
    }

    void CameraCtrlStandard::update(float _deltaTime, const IVector3& _mouseState, bool mouseDown, bool keys[CamCtrlCount])
    {
        if (!mouseDown)
        {
            m_mouseLast.m_mx = _mouseState.x;
            m_mouseLast.m_my = _mouseState.y;
            m_mouseLast.m_mz = _mouseState.z;
            m_mouseNow = m_mouseLast;
            m_mouseDown = false;

            return;
        }

        if (!m_mouseDown)
        {
            m_mouseLast.m_mx = _mouseState.x;
            m_mouseLast.m_my = _mouseState.y;
        }

        m_mouseDown = mouseDown;

        if (m_mouseDown)
        {
            m_mouseNow.m_mx = _mouseState.x;
            m_mouseNow.m_my = _mouseState.y;
        }

        m_mouseLast.m_mz = m_mouseNow.m_mz;
        m_mouseNow.m_mz = _mouseState.z;

        const float deltaZ = float(m_mouseNow.m_mz - m_mouseLast.m_mz);

        if (m_mouseDown)
        {
            const int32_t deltaX = m_mouseNow.m_mx - m_mouseLast.m_mx;
            const int32_t deltaY = m_mouseNow.m_my - m_mouseLast.m_my;

            m_horizontalAngle += m_mouseSpeed * float(deltaX);
            m_verticalAngle -= m_mouseSpeed * float(deltaY);

            m_mouseLast.m_mx = m_mouseNow.m_mx;
            m_mouseLast.m_my = m_mouseNow.m_my;
        }

        const Vector3 direction =
        {
            glm::cos(m_verticalAngle) * glm::sin(m_horizontalAngle),
            glm::sin(m_verticalAngle),
            glm::cos(m_verticalAngle) * glm::cos(m_horizontalAngle),
        };

        const Vector3 right =
        {
            glm::sin(m_horizontalAngle - glm::half_pi<float>()),
            0.0f,
            glm::cos(m_horizontalAngle - glm::half_pi<float>()),
        };

        const Vector3 up = glm::cross(right, direction);

        m_eye = mad(direction, deltaZ * _deltaTime * m_moveSpeed, m_eye);

        if (keys[CamCtrlForward])
        {
            m_eye = mad(direction, _deltaTime * m_moveSpeed, m_eye);
        }

        if (keys[CamCtrlBackward])
        {
            m_eye = mad(direction, -_deltaTime * m_moveSpeed, m_eye);
        }

        if (keys[CamCtrlLeft])
        {
            m_eye = mad(right, _deltaTime * m_moveSpeed, m_eye);
        }

        if (keys[CamCtrlRight])
        {
            m_eye = mad(right, -_deltaTime * m_moveSpeed, m_eye);
        }

        if (keys[CamCtrlUp])
        {
            m_eye = mad(up, _deltaTime * m_moveSpeed, m_eye);
        }

        if (keys[CamCtrlDown])
        {
            m_eye = mad(up, -_deltaTime * m_moveSpeed, m_eye);
        }

        m_at = m_eye + direction;
        m_up = glm::cross(right, direction);

        Matrix4 view;
        auto camera = Get<Camera>();
        getViewMtx(toFloatPtr(view));
        camera->UpdateFromViewMatrix(view);
    }

    void CameraCtrlStandard::getViewMtx(float* _viewMtx)
    {
        memcpy(_viewMtx, &glm::lookAt(m_eye, m_at, m_up), sizeof(float) * 16);
    }

    void CameraCtrlStandard::setPosition(const Vector3& _pos)
    {
        m_eye = _pos;
    }

    void CameraCtrlStandard::setVerticalAngle(float _verticalAngle)
    {
        m_verticalAngle = _verticalAngle;
    }

    void CameraCtrlStandard::setHorizontalAngle(float _horizontalAngle)
    {
        m_horizontalAngle = _horizontalAngle;
    }

}