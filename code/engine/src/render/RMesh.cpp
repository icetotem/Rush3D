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

        if (!ret || !err.empty())
        {
            LOG_ERROR("Failed to parse glb: {}, reason: {}", path, err.empty());
            return false;
        }

        Ref<RVertexBuffer> positionBuffer, texcoordBuffer;
        Ref<RIndexBuffer> indexBuffer;

        // 遍历模型的所有网格
        for (size_t i = 0; i < model.meshes.size(); ++i) {
            const tinygltf::Mesh& mesh = model.meshes[i];

            // 遍历每个网格的子网格（primitive）
            for (size_t j = 0; j < mesh.primitives.size(); ++j) {
                const tinygltf::Primitive& primitive = mesh.primitives[j];

                // 访问顶点坐标
                if (primitive.attributes.find("POSITION") != primitive.attributes.end()) {
                    int accessorIdx = primitive.attributes.at("POSITION");
                    const tinygltf::Accessor& accessor = model.accessors[accessorIdx];
                    const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                    const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                    // 访问实际的顶点数据
                    const float* vertexData = reinterpret_cast<const float*>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
                    int numVertices = accessor.count;
                    int numComponents = accessor.type == TINYGLTF_TYPE_VEC3 ? 3 : 2; // 2 for texture coordinates, 3 for positions
                    uint32_t size = numVertices * numComponents * sizeof(float);
                    positionBuffer = CreateRef<RVertexBuffer>(sizeof(float) * 3, size, vertexData, "position");
                }

                // 访问法线数据
                if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
                    int accessorIdx = primitive.attributes.at("NORMAL");
                    const tinygltf::Accessor& accessor = model.accessors[accessorIdx];
                    const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                    const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                    // 访问法线数据的方式与访问顶点坐标类似
                    // 你可以在这里遍历法线数据并使用它们
                }

                // 访问切线数据
                if (primitive.attributes.find("TANGENT") != primitive.attributes.end()) {
                    int tangentAccessorIdx = primitive.attributes.at("TANGENT");
                    const tinygltf::Accessor& tangentAccessor = model.accessors[tangentAccessorIdx];
                    const tinygltf::BufferView& tangentBufferView = model.bufferViews[tangentAccessor.bufferView];
                    const tinygltf::Buffer& tangentBuffer = model.buffers[tangentBufferView.buffer];
                    const float* tangents = reinterpret_cast<const float*>(&(tangentBuffer.data[tangentAccessor.byteOffset + tangentBufferView.byteOffset]));

                    // 在这里访问切线数据（tangents）
                }

                // 访问纹理坐标数据
                if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
                    int accessorIdx = primitive.attributes.at("TEXCOORD_0");
                    const tinygltf::Accessor& accessor = model.accessors[accessorIdx];
                    const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                    const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                    const float* vertexData = reinterpret_cast<const float*>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
                    int numVertices = accessor.count;
                    int numComponents = accessor.type == TINYGLTF_TYPE_VEC3 ? 3 : 2; // 2 for texture coordinates, 3 for positions
                    uint32_t size = numVertices * numComponents * sizeof(float);
                    texcoordBuffer = CreateRef<RVertexBuffer>(sizeof(float) * 2, size, vertexData, "texcoord");
                }

                // 访问骨骼索引和权重数据
                if (primitive.attributes.find("JOINTS_0") != primitive.attributes.end() &&
                    primitive.attributes.find("WEIGHTS_0") != primitive.attributes.end()) {
                    int jointsAccessorIdx = primitive.attributes.at("JOINTS_0");
                    int weightsAccessorIdx = primitive.attributes.at("WEIGHTS_0");

                    const tinygltf::Accessor& jointsAccessor = model.accessors[jointsAccessorIdx];
                    const tinygltf::Accessor& weightsAccessor = model.accessors[weightsAccessorIdx];
                    const tinygltf::BufferView& jointsBufferView = model.bufferViews[jointsAccessor.bufferView];
                    const tinygltf::BufferView& weightsBufferView = model.bufferViews[weightsAccessor.bufferView];
                    const tinygltf::Buffer& jointsBuffer = model.buffers[jointsBufferView.buffer];
                    const tinygltf::Buffer& weightsBuffer = model.buffers[weightsBufferView.buffer];

                    const uint16_t* jointIndices = reinterpret_cast<const uint16_t*>(&(jointsBuffer.data[jointsAccessor.byteOffset + jointsBufferView.byteOffset]));
                    const float* jointWeights = reinterpret_cast<const float*>(&(weightsBuffer.data[weightsAccessor.byteOffset + weightsBufferView.byteOffset]));

                    // 在这里访问骨骼索引和权重数据
                    // jointIndices 包含骨骼索引，jointWeights 包含权重数据
                }

                // 获取子网格的索引缓冲区
                if (primitive.indices >= 0) {
                    const tinygltf::Accessor& accessor = model.accessors[primitive.indices];
                    const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                    const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                    // 访问索引数据
                    const uint16_t* indices = reinterpret_cast<const uint16_t*>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
                    int numIndices = accessor.count;

                    indexBuffer = CreateRef<RIndexBuffer>(numIndices, IndexFormat::Uint16, indices, "index");
                }
            }
        }

        auto& subMesh = primitives.emplace_back();
        subMesh.vertexBuffers.push_back(positionBuffer);
        subMesh.vertexBuffers.push_back(texcoordBuffer);
        subMesh.indexBuffer = indexBuffer;

//         // create the buffers (x, y)
//         float const vertData0[] = {
//             -0.8f, -0.8f, // BL
//              0.8f, -0.8f, // BR
//              0.8f,  0.8f, // TR
//             -0.8f,  0.8f, // TL
//         };
// 
//         // create the buffers (r, g, b)
//         float const vertData1[] = {
//             0.0f, 1.0f, // BL
//             1.0f, 1.0f, // BR
//             1.0f, 0.0f, // TR
//             0.0f, 0.0f, // TL
//         };
// 
//         uint16_t const indxData[] = {
//             0, 1, 2, 0, 2, 3
//         };
// 
//         Ref<RVertexBuffer> vb0 = CreateRef<RVertexBuffer>(sizeof(float) * 2, sizeof(vertData0), "vb0");
//         vb0->UpdateData(vertData0, sizeof(vertData0));
// 
//         Ref<RVertexBuffer> vb1 = CreateRef<RVertexBuffer>(sizeof(float) * 2, sizeof(vertData1), "vb1");
//         vb1->UpdateData(vertData1, sizeof(vertData1));
// 
//         Ref<RIndexBuffer> ib = CreateRef<RIndexBuffer>(6, false, "ib0");
//         ib->UpdateData(indxData, sizeof(indxData));
// 
//         subMesh.vertexBuffers.push_back(vb0);
//         subMesh.vertexBuffers.push_back(vb1);
//         subMesh.indexBuffer = ib;

        return true;
    }

}