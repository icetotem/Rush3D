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

        // ����ģ�͵���������
        for (size_t i = 0; i < model.meshes.size(); ++i) 
        {
            Ref<RVertexBuffer> positionBuffer, texcoordBuffer;
            Ref<RIndexBuffer> indexBuffer;

            const tinygltf::Mesh& mesh = model.meshes[i];

            // ����ÿ�������������primitive��
            for (size_t j = 0; j < mesh.primitives.size(); ++j) 
            {
                const tinygltf::Primitive& primitive = mesh.primitives[j];

                // ���ʶ�������
                if (primitive.attributes.find("POSITION") != primitive.attributes.end()) 
                {
                    int accessorIdx = primitive.attributes.at("POSITION");
                    const tinygltf::Accessor& accessor = model.accessors[accessorIdx];
                    const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                    const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                    // ����ʵ�ʵĶ�������
                    const float* vertexData = reinterpret_cast<const float*>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
                    int numVertices = accessor.count;
                    int numComponents = accessor.type == TINYGLTF_TYPE_VEC3 ? 3 : 2; // 2 for texture coordinates, 3 for positions
                    uint32_t size = numVertices * numComponents * sizeof(float);
                    positionBuffer = CreateRef<RVertexBuffer>(sizeof(float) * 3, size, vertexData, "position");
                }

                // ���ʷ�������
                if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) 
                {
                    int accessorIdx = primitive.attributes.at("NORMAL");
                    const tinygltf::Accessor& accessor = model.accessors[accessorIdx];
                    const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                    const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                    // ���ʷ������ݵķ�ʽ����ʶ�����������
                    // ���������������������ݲ�ʹ������
                }

                // ������������
                if (primitive.attributes.find("TANGENT") != primitive.attributes.end())
                {
                    int tangentAccessorIdx = primitive.attributes.at("TANGENT");
                    const tinygltf::Accessor& tangentAccessor = model.accessors[tangentAccessorIdx];
                    const tinygltf::BufferView& tangentBufferView = model.bufferViews[tangentAccessor.bufferView];
                    const tinygltf::Buffer& tangentBuffer = model.buffers[tangentBufferView.buffer];
                    const float* tangents = reinterpret_cast<const float*>(&(tangentBuffer.data[tangentAccessor.byteOffset + tangentBufferView.byteOffset]));

                    // ����������������ݣ�tangents��
                }

                // ����������������
                if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) 
                {
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

                // ���ʹ���������Ȩ������
                if (primitive.attributes.find("JOINTS_0") != primitive.attributes.end() &&
                    primitive.attributes.find("WEIGHTS_0") != primitive.attributes.end()) 
                {
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

                    // ��������ʹ���������Ȩ������
                    // jointIndices ��������������jointWeights ����Ȩ������
                }

                // ��ȡ�����������������
                if (primitive.indices >= 0) 
                {
                    const tinygltf::Accessor& accessor = model.accessors[primitive.indices];
                    const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                    const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                    // ������������
                    const uint16_t* indices = reinterpret_cast<const uint16_t*>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
                    int numIndices = accessor.count;

                    indexBuffer = CreateRef<RIndexBuffer>(numIndices, IndexFormat::Uint16, indices, "index");
                }
            }

            auto& geo = CreateRef<RGeometry>();
            geo->vertexBuffers.push_back(positionBuffer);
            geo->vertexBuffers.push_back(texcoordBuffer);
            geo->indexBuffer = indexBuffer;
            auto& subMesh = m_Submeshes.emplace_back();
            subMesh.geometry = geo;
            subMesh.material = "default_mat";
        }
        return true;
    }

}