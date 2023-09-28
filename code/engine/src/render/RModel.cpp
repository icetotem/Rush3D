#include "stdafx.h"

// Define these only in *one* .cc file.
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
// #define TINYGLTF_NOEXCEPTION // optional. disable exception handling.
#undef STB_IMAGE_IMPLEMENTATION // already defined in RTexture.cpp
#include <gltf/tiny_gltf.h>

#include "render/RModel.h"
#include "BundleManager.h"

namespace rush
{

    bool RModel::Load(const StringView& path)
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

        // 遍历模型的所有网格
        for (size_t i = 0; i < model.meshes.size(); ++i) 
        {
            auto& inner_mesh = m_Meshes.emplace_back();

            const tinygltf::Mesh& mesh = model.meshes[i];

            // 遍历每个网格的子网格（primitive）
            for (size_t j = 0; j < mesh.primitives.size(); ++j) 
            {

                VertexLayout vlayouts[kMaxVertexBuffers];
                const float* positions = nullptr;
                const float* normals = nullptr;
                const float* tangents = nullptr;
                const float* texcoords0 = nullptr;
                const uint16_t* jointIndices = nullptr;
                const float* jointWeights = nullptr;
                int numVertices = 0;
                int numIndices = 0;
                const uint16_t* indices = nullptr;

                const tinygltf::Primitive& primitive = mesh.primitives[j];
                if (primitive.attributes.find("POSITION") == primitive.attributes.end())
                {
                    LOG_WARN("mesh {} has no positon data", path);
                    continue;
                }

                int buffIndex = 0;
                // 访问顶点坐标
                {
                    int accessorIdx = primitive.attributes.at("POSITION");
                    const tinygltf::Accessor& accessor = model.accessors[accessorIdx];
                    numVertices = accessor.count;
                    const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                    const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                    if (accessor.type != TINYGLTF_TYPE_VEC3)
                    {
                        LOG_WARN("mesh {} positon data type error", path);
                        continue;
                    }

                    positions = reinterpret_cast<const float*>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
                    vlayouts[buffIndex].attributeCount = 1;
                    vlayouts[buffIndex].stride = 3 * sizeof(float);
                    vlayouts[buffIndex].attributes[0].format = VertexFormat::Float32x3;
                    vlayouts[buffIndex].attributes[0].offset = 0;
                    vlayouts[buffIndex].attributes[0].location = buffIndex;
                    ++buffIndex;
                }

                // 访问法线数据
                if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) 
                {
                    int accessorIdx = primitive.attributes.at("NORMAL");
                    const tinygltf::Accessor& accessor = model.accessors[accessorIdx];
                    const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                    const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                    if (accessor.type != TINYGLTF_TYPE_VEC3)
                    {
                        LOG_WARN("mesh {} normal data type error", path);
                    }
                    else
                    {
                        normals = reinterpret_cast<const float*>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
                        vlayouts[buffIndex].attributeCount = 1;
                        vlayouts[buffIndex].stride = 3 * sizeof(float);
                        vlayouts[buffIndex].attributes[0].format = VertexFormat::Float32x3;
                        vlayouts[buffIndex].attributes[0].offset = 0;
                        vlayouts[buffIndex].attributes[0].location = buffIndex;
                        ++buffIndex;
                    }
                }

                // 访问切线数据
                if (primitive.attributes.find("TANGENT") != primitive.attributes.end())
                {
                    int tangentAccessorIdx = primitive.attributes.at("TANGENT");
                    const tinygltf::Accessor& accessor = model.accessors[tangentAccessorIdx];
                    const tinygltf::BufferView& tangentBufferView = model.bufferViews[accessor.bufferView];
                    const tinygltf::Buffer& tangentBuffer = model.buffers[tangentBufferView.buffer];

                    if (accessor.type != TINYGLTF_TYPE_VEC3)
                    {
                        LOG_WARN("mesh {} tangent data type error", path);
                    }
                    else
                    {
                        tangents = reinterpret_cast<const float*>(&(tangentBuffer.data[accessor.byteOffset + tangentBufferView.byteOffset]));
                        vlayouts[buffIndex].attributeCount = 1;
                        vlayouts[buffIndex].stride = 3 * sizeof(float);
                        vlayouts[buffIndex].attributes[0].format = VertexFormat::Float32x3;
                        vlayouts[buffIndex].attributes[0].offset = 0;
                        vlayouts[buffIndex].attributes[0].location = buffIndex;
                        ++buffIndex;
                    }
                }

                // 访问纹理坐标数据
                if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) 
                {
                    int accessorIdx = primitive.attributes.at("TEXCOORD_0");
                    const tinygltf::Accessor& accessor = model.accessors[accessorIdx];
                    const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                    const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                    if (accessor.type != TINYGLTF_TYPE_VEC2)
                    {
                        LOG_WARN("mesh {} texture coord 0 data type error", path);
                        continue;
                    }
                    else
                    {
                        texcoords0 = reinterpret_cast<const float*>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
                        vlayouts[buffIndex].attributeCount = 1;
                        vlayouts[buffIndex].stride = 2 * sizeof(float);
                        vlayouts[buffIndex].attributes[0].format = VertexFormat::Float32x2;
                        vlayouts[buffIndex].attributes[0].offset = 0;
                        vlayouts[buffIndex].attributes[0].location = buffIndex;
                        ++buffIndex;
                    }
                }

                // 访问骨骼索引和权重数据
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

                    if (jointsAccessor.type != TINYGLTF_TYPE_VEC4 || weightsAccessor.type != TINYGLTF_TYPE_VEC4)
                    {
                        LOG_WARN("mesh {} joint index data type error", path);
                        continue;
                    }
                    else
                    {
                        const uint16_t* jointIndices = reinterpret_cast<const uint16_t*>(&(jointsBuffer.data[jointsAccessor.byteOffset + jointsBufferView.byteOffset]));
                        vlayouts[buffIndex].attributeCount = 1;
                        vlayouts[buffIndex].stride = 4 * sizeof(uint16_t);
                        vlayouts[buffIndex].attributes[0].format = VertexFormat::Uint16x4;
                        vlayouts[buffIndex].attributes[0].offset = 0;
                        vlayouts[buffIndex].attributes[0].location = buffIndex;
                        ++buffIndex;

                        const float* jointWeights = reinterpret_cast<const float*>(&(weightsBuffer.data[weightsAccessor.byteOffset + weightsBufferView.byteOffset]));
                        vlayouts[buffIndex].attributeCount = 1;
                        vlayouts[buffIndex].stride = 4 * sizeof(float);
                        vlayouts[buffIndex].attributes[0].format = VertexFormat::Float32x4;
                        vlayouts[buffIndex].attributes[0].offset = 0;
                        vlayouts[buffIndex].attributes[0].location = buffIndex;
                        ++buffIndex;
                    }
                }

                // 获取子网格的索引缓冲区
                if (primitive.indices >= 0)
                {
                    const tinygltf::Accessor& accessor = model.accessors[primitive.indices];
                    const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                    const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                    // 访问索引数据
                    indices = reinterpret_cast<const uint16_t*>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
                    numIndices = accessor.count;
                }

                auto& inner_primitive = inner_mesh.primitives.emplace_back();
                auto geo = CreateRef<RGeometry>(PrimitiveTopology::TriangleList, buffIndex, vlayouts, numVertices, numIndices, path);

                buffIndex = 0;
                if (positions)
                {
                    geo->UpdateVertexBuffer(buffIndex++, positions, numVertices * sizeof(float) * 3);
                }
                if (normals)
                {
                    geo->UpdateVertexBuffer(buffIndex++, normals, numVertices * sizeof(float) * 3);
                }
                if (tangents)
                {
                    geo->UpdateVertexBuffer(buffIndex++, tangents, numVertices * sizeof(float) * 3);
                }
                if (texcoords0)
                {
                    geo->UpdateVertexBuffer(buffIndex++, texcoords0, numVertices * sizeof(float) * 2);
                }
                if (jointIndices && jointWeights)
                {
                    geo->UpdateVertexBuffer(buffIndex++, jointIndices, numVertices * sizeof(uint16_t) * 4);
                    geo->UpdateVertexBuffer(buffIndex++, jointWeights, numVertices * sizeof(float) * 4);
                }
                if (indices)
                {
                    geo->UpdateIndexBuffer(indices, numIndices * sizeof(uint16_t), 0);
                }

                inner_primitive.geometry = geo;
                inner_primitive.material = "";//primitive.material;
            }

        }
        return true;
    }

}