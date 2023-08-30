#include "stdafx.h"
#include "components/Camera.h"
#include "components/Transform.h"

namespace rush
{

    Camera::Camera(Entity owner)
        : Component(owner)
        , m_Fov(60.0f), m_Aspect(1.33333f), m_NearClip(0.1f), m_FarClip(1000.0f)
        , m_ProjMatrix(glm::identity<Matrix4>())
        , m_ViewMatrix(glm::identity<Matrix4>())
    {
    }

    void Camera::SetRenderer(Ref<Renderer> renderer)
    {
        m_Renderer = renderer;
        uint32_t width, height;
        GetViewSize(width, height);
        m_Aspect = (float)width / (float)height;
    }

    void Camera::SetFov(float fov)
    {
        m_Fov = fov;
    }

    void Camera::SetClip(float nearClip, float farClip)
    {
        m_NearClip = nearClip;
        m_FarClip = farClip;
    }

    void Camera::SetViewport(float x, float y, float width, float height)
    {
        m_Viewport = { x, y, width, height };
        uint32_t rtWidth, rtHeight;
        GetViewSize(rtWidth, rtHeight);
        m_Aspect = (float)width / (float)height;
    }

    void Camera::GetViewSize(uint32_t& width, uint32_t& height)
    {
        width = m_Renderer->GetWidth() * (m_Viewport.z - m_Viewport.x);
        height = m_Renderer->GetHeight() * (m_Viewport.w - m_Viewport.y);
    }

    void Camera::GetViewCorner(uint32_t& x, uint32_t& y)
    {
        x = m_Renderer->GetWidth() * m_Viewport.x;
        y = m_Renderer->GetHeight() * m_Viewport.y;
    }

    void Camera::UpdateMatrix()
    {
        auto transform = Get<Transform>();
        const Vector3& eyePoint = transform->GetPosition();
        Vector3 lookPoint = transform->GetPosition() + transform->GetForward();
        const Vector3& camUp = transform->GetUp();
        m_ViewMatrix = glm::lookAt(eyePoint, lookPoint, camUp);

        uint32_t rtWidth, rtHeight;
        GetViewSize(rtWidth, rtHeight);
        m_ProjMatrix = glm::perspectiveRH(m_Fov, m_Aspect, m_NearClip, m_FarClip);
    }

    void Camera::UpdateFromViewMatrix(const Matrix4& viewMatrix)
    {
        auto invView = glm::inverse(viewMatrix);
        Vector3 eyePoint = Vector3(invView[3][0], invView[3][1], invView[3][2]);
        auto transform = Get<Transform>();
        transform->SetPosition(eyePoint);
        Vector3 forward = -Vector3(invView[2][0], invView[2][1], invView[2][2]);
        transform->LookAt(eyePoint + forward);
    }

    Ray Camera::GetScreenCenterRay()
    {
        Ray ray;
        auto transform = Get<Transform>();
        ray.origin = transform->GetPosition();
        ray.direction = transform->GetForward();
        return ray;
    }

    Ray Camera::GetScreenRay(int x, int y)
    {
        uint32_t screenWidth, screenHeight;
        GetViewSize(screenWidth, screenHeight);

        glm::vec2 normalizedScreenPoint = glm::vec2((float)x / (float)screenWidth, ((float)screenHeight - (float)y) / (float)screenHeight);
        glm::vec2 viewportPoint = 2.0f * normalizedScreenPoint - 1.0f;
        glm::mat4 invViewProjMatrix = glm::inverse(m_ProjMatrix * m_ViewMatrix);
        glm::vec4 rayStart(viewportPoint, 0.0f, 1.0f);
        glm::vec4 rayEnd(viewportPoint, 1.0f, 1.0f);
        glm::vec4 rayStartWorld = invViewProjMatrix * rayStart;
        rayStartWorld /= rayStartWorld.w;
        glm::vec4 rayEndWorld = invViewProjMatrix * rayEnd;
        rayEndWorld /= rayEndWorld.w;

        Ray ray;
        ray.origin = rayStartWorld;
        ray.direction = glm::normalize(glm::vec3(rayEndWorld - rayStartWorld));
        return ray;
    }


}