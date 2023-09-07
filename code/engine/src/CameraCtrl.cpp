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
            if (code == KeyCode::A)
            {
                m_MovingLeft = (state != InputButtonState::Released);
            }
            else if (code == KeyCode::S)
            {
                m_MovingBackward = (state != InputButtonState::Released);
            }
            else if (code == KeyCode::D)
            {
                m_MovingRight = (state != InputButtonState::Released);
            }
            else if (code == KeyCode::W)
            {
                m_MovingForward = (state != InputButtonState::Released);
            }
            else if (code == KeyCode::Q)
            {
                m_MovingDown = (state != InputButtonState::Released);
            }
            else if (code == KeyCode::E)
            {
                m_MovingUp = (state != InputButtonState::Released);
            }
            else if (code == KeyCode::LeftShift)
            {
                m_BoostMove = (state != InputButtonState::Released);
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

        window->BindMouseWheelCallback([&](uint32_t delta)->bool {
            Zoom(delta);
            return false;
        });
    }

    void CameraCtrlFirstPerson::Update()
    {
        double deltaSec = Timer::GetDeltaTimeSec();
        float delta = deltaSec * m_MoveSpeed * (m_BoostMove ? 2.0f : 1.0f);
        auto transform = m_Camera.Get<Transform>();

        if (m_MovingLeft)
        {
            transform->Translate({ -delta, 0, 0 });
        }

        if (m_MovingRight)
        {
            transform->Translate({ delta, 0, 0 });
        }

        if (m_MovingForward)
        {
            transform->Translate({ 0, 0, delta });
        }

        if (m_MovingBackward)
        {
            transform->Translate({ 0, 0, -delta });
        }

        if (m_MovingUp)
        {
            transform->Translate({ 0, delta, 0 }, Space::World);
        }

        if (m_MovingDown)
        {
            transform->Translate({ 0, -delta, 0 }, Space::World);
        }

        auto camera = m_Camera.Get<Camera>();
        camera->UpdateViewMatrix();
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
            const auto& deltaPitch = glm::angleAxis(degToRad(deltaY), transform->GetRight());

            auto forward = transform->GetForward();
            forward = glm::rotate(deltaPitch * deltaYaw, forward);
            auto right = glm::normalize(glm::cross(yAxis, forward));
            auto newRot = quatFromXZ(right, forward);
            transform->SetRotation(newRot);

            // update camera view matrix
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