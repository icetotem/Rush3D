#include "stdafx.h"
#include "components/Geometry.h"
#include "render/Renderer.h"

namespace rush
{  

    Geometry::Geometry(Entity owner)
        : Component(owner)
    {
    }

    Geometry::~Geometry()
    {
    }

    void Geometry::SetAsset(const StringView& path)
    {
        // create the buffers (x, y)
        float const vertData0[] = {
            -0.8f, -0.8f, // BL
             0.8f, -0.8f, // BR
            -0.0f,  0.8f, // top
        };

        // create the buffers (r, g, b)
        float const vertData1[] = {
            0.7f, 0.7f, 0.7f, // BL
            0.7f, 0.7f, 0.7f, // BR
            0.7f, 0.7f, 0.7f, // top
        };

        uint16_t const indxData[] = {
            0, 1, 2, 4
        };

        Ref<RVertexBuffer> vb0 = CreateRef<RVertexBuffer>(sizeof(float) * 2, sizeof(vertData0), "vb0");
        vb0->UpdateData(vertData0, sizeof(vertData0));

        Ref<RVertexBuffer> vb1 = CreateRef<RVertexBuffer>(sizeof(float) * 3, sizeof(vertData1), "vb1");
        vb1->UpdateData(vertData1, sizeof(vertData1));

        Ref<RIndexBuffer> ib = CreateRef<RIndexBuffer>(4, false, "ib0");
        ib->UpdateData(indxData, sizeof(indxData));

        m_VertexBuffers.push_back(vb0);
        m_VertexBuffers.push_back(vb1);

        if (m_IndexBuffer == nullptr)
            m_IndexBuffer = ib;
    }

}
