#ifndef Camera_h__
#define Camera_h__

#include "components/EcsSystem.h"
#include "render/RenderContext.h"

namespace rush
{

    class Camera : public Component
    {
    public:
        Camera(Entity owner);

        void SetRenderer(Ref<RenderContext> renderer);
        Ref<RenderContext> GetRenderer() const { return m_Renderer; }

        void SetFov(float fov);
        void SetClip(float nearClip, float farClip);
         
        // 0 ~ 1
        void SetViewport(float x, float y, float width, float height);
        void SetViewport(const Vector4& viewport) { m_Viewport = viewport; }
        const Vector4 GetViewport() const { return m_Viewport; }

        void SetClearColor(const Vector4& color) { m_ClearColor = color; }
        const Vector4& GetClearColor() const { return m_ClearColor; }

        void SetClearDepth(float depth) { m_ClearDepth = depth; }
        float GetClearDepth() const { return m_ClearDepth; }

        void UpdateViewMatrix();
        // for gizmos
        void UpdateViewMatrix(const Matrix4& viewMatrix);

        Ray GetScreenCenterRay() const;
        Ray GetScreenRay(int x, int y) const;

        const Matrix4& GetViewMatrix() const { return m_ViewMatrix; }
        const Matrix4& GetProjMatrix() const { return m_ProjMatrix; }

        float GetFov() const { return m_Fov; }
        float GetAspect() const { return m_Aspect; }
        float GetNearClip() const { return m_NearClip; }
        float GetFarClip() const { return m_FarClip; }

        // get real size in pixels
        void GetViewSize(uint32_t& width, uint32_t& height) const;

        // get left-top corner position in pixels
        void GetViewCorner(uint32_t& x, uint32_t& y) const;

    protected:
        void UpdateProjMatrix();
        void UpdateAspect();

    protected:
        Ref<RenderContext> m_Renderer;
        Vector4 m_ClearColor = { 0.2f, 0.2f, 0.2f, 1.0f };
        Vector4 m_Viewport = {0.0f, 0.0f, 1.0f, 1.0f};
        float m_ClearDepth = 1.0f;
        uint8_t m_Stencil = 0;
        float m_Fov, m_Aspect, m_NearClip, m_FarClip;
        Matrix4 m_ViewMatrix, m_ProjMatrix;
    };
}

#endif // Camera_h__
