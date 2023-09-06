#include "stdafx.h"
#include "CameraCtrl.h"
#include "components/Transform.h"
#include "components/Camera.h"

namespace rush
{

    CameraCtrlFirstPerson::CameraCtrlFirstPerson()
    {

    }

    void CameraCtrlFirstPerson::Setup(Ref<Window> window, Entity camera)
    {
        m_Camera = camera;

        window->BindKeyCallback([=](InputButtonState state, KeyCode code)->bool {
            if (state == InputButtonState::Pressed)
            {
                camera.Get<Camera>();
                if (code == KeyCode::A)
                {
                }
                else if (code == KeyCode::S)
                {
                }
                else if (code == KeyCode::D)
                {
                }
                else if (code == KeyCode::W)
                {
                }
                else if (code == KeyCode::Q)
                {
                }
                else if (code == KeyCode::E)
                {
                }
            }
            return false;
        });

        window->BindMouseButtonCallback([&](InputButtonState state, MouseCode code, uint32_t mouseX, uint32_t mouseY)->bool {
            if (code == MouseCode::ButtonRight)
            {
                if (state == InputButtonState::Pressed)
                {
                    m_Rotating = true;
                }
                else if (state == InputButtonState::Released)
                {
                    m_Rotating = false;
                }
            }

            return false;
        });

        window->BindMouseMoveCallback([&](uint32_t x, uint32_t y)->bool {
            OnMouseMove(x, y);
            return false;
        });

        window->BindMouseWheelCallback([&](uint32_t v, uint32_t h)->bool {
            Zoom(v);
            return false;
        });
    }

    void CameraCtrlFirstPerson::OnMouseMove(int x, int y)
    {
        double deltaSec = Timer::GetDeltaTimeSec();
        if (m_Rotating)
        {
            auto transform = m_Camera.Get<Transform>();

            int mouseX = x;
            int mouseY = y;

            auto pitch = transform->GetLocalEulerAngles().x;

            float safePitch = 85.0f;

            float deltaX = (float)(mouseX - m_LastMouseX) * deltaSec * m_OrbitSpeed;
            float deltaY = (float)(mouseY - m_LastMouseY) * deltaSec * m_OrbitSpeed;

            if (pitch + deltaY > safePitch)
            {
                deltaY = pitch - safePitch;
            }
            else if (pitch + deltaY < -safePitch)
            {
                deltaY = pitch + safePitch;
            }

            const auto& deltaYaw = glm::angleAxis(degToRad(deltaX), yAxis);
            const auto& deltaPitch = glm::angleAxis(degToRad(deltaY), transform->GetLeft());

            auto towardsCam = -transform->GetForward();
            towardsCam = glm::rotate(deltaPitch * deltaYaw, towardsCam);

            auto right = glm::normalize(glm::cross(yAxis, -towardsCam));

            auto newRot = quatFromXZ(right, -towardsCam);

            transform->SetRotation(newRot);

            auto camera = m_Camera.Get<Camera>();
            camera->UpdateViewMatrix();
        }

        m_LastMouseX = x;
        m_LastMouseY = y;
    }

    void CameraCtrlFirstPerson::Zoom(int value)
    {
        double deltaSec = Timer::GetDeltaTimeSec();
        float delta = value * deltaSec * m_ZoomSpeed;
        auto transform = m_Camera.Get<Transform>();
        transform->Translate({ 0, 0, delta });
    }

}