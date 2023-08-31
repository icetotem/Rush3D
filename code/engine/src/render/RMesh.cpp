#include "stdafx.h"
#include "render/RMesh.h"

namespace rush
{

    bool RMesh::Load(const StringView& path)
    {
        auto& subMesh = subMeshes.emplace_back();

        // create the buffers (x, y)
        float const vertData0[] = {
            -0.8f, -0.8f, // BL
             0.8f, -0.8f, // BR
            -0.0f,  0.8f, // top
        };

        // create the buffers (r, g, b)
        float const vertData1[] = {
            0.0f, 0.0f, // BL
            0.0f, 1.0f, // BR
            1.0f, 1.0f, // top
        };

        uint16_t const indxData[] = {
            0, 1, 2, 4
        };

        Ref<RVertexBuffer> vb0 = CreateRef<RVertexBuffer>(sizeof(float) * 2, sizeof(vertData0), "vb0");
        vb0->UpdateData(vertData0, sizeof(vertData0));

        Ref<RVertexBuffer> vb1 = CreateRef<RVertexBuffer>(sizeof(float) * 2, sizeof(vertData1), "vb1");
        vb1->UpdateData(vertData1, sizeof(vertData1));

        Ref<RIndexBuffer> ib = CreateRef<RIndexBuffer>(4, false, "ib0");
        ib->UpdateData(indxData, sizeof(indxData));

        subMesh.vertexBuffers.push_back(vb0);
        subMesh.vertexBuffers.push_back(vb1);
        subMesh.indexBuffer = ib;

        return true;
    }

}