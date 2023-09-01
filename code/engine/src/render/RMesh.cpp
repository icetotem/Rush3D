#include "stdafx.h"

// Define these only in *one* .cc file.
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
// #define TINYGLTF_NOEXCEPTION // optional. disable exception handling.
#undef STB_IMAGE_IMPLEMENTATION // already defined in RTexture.cpp
#include <gltf/tiny_gltf.h>

#include "render/RMesh.h"
#include "BundleManager.h"

namespace rush
{

    bool RMesh::Load(const StringView& path)
    {
        auto stream = BundleManager::instance().Get(path);
        if (stream->IsEmpty())
        {
            return false;
        }

        using namespace tinygltf;

        Model model;
        TinyGLTF loader;
        std::string err;
        std::string warn;

        bool ret = loader.LoadBinaryFromMemory(&model, &err, &warn, stream->GetData(), stream->GetSize());
        if (!warn.empty()) 
        {
            LOG_WARN("Load glb Warning: {}", warn);
        }

        if (!err.empty())
        {
            LOG_WARN("Load glb Error: {}", err);
        }

        if (!ret)
        {
            LOG_WARN("Failed to parse glb: {}", path);
            return false;
        }

        auto& subMesh = subMeshes.emplace_back();

        // create the buffers (x, y)
        float const vertData0[] = {
            -0.8f, -0.8f, // BL
             0.8f, -0.8f, // BR
             0.8f,  0.8f, // TR
            -0.8f,  0.8f, // TL
        };

        // create the buffers (r, g, b)
        float const vertData1[] = {
            0.0f, 1.0f, // BL
            1.0f, 1.0f, // BR
            1.0f, 0.0f, // TR
            0.0f, 0.0f, // TL
        };

        uint16_t const indxData[] = {
            0, 1, 2, 0, 2, 3
        };

        Ref<RVertexBuffer> vb0 = CreateRef<RVertexBuffer>(sizeof(float) * 2, sizeof(vertData0), "vb0");
        vb0->UpdateData(vertData0, sizeof(vertData0));

        Ref<RVertexBuffer> vb1 = CreateRef<RVertexBuffer>(sizeof(float) * 2, sizeof(vertData1), "vb1");
        vb1->UpdateData(vertData1, sizeof(vertData1));

        Ref<RIndexBuffer> ib = CreateRef<RIndexBuffer>(6, false, "ib0");
        ib->UpdateData(indxData, sizeof(indxData));

        subMesh.vertexBuffers.push_back(vb0);
        subMesh.vertexBuffers.push_back(vb1);
        subMesh.indexBuffer = ib;

        return true;
    }

}