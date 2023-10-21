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

    static void CalculateTangentArray(long vertexCount, const Vector3* vertex, const Vector3* normal,
        const Vector2* texcoord, long triangleCount, const uint16_t* triangle, Vector3* tangent)
    {
        Vector3* tan1 = new Vector3[vertexCount * 2];
        Vector3* tan2 = tan1 + vertexCount;
        ZeroMemory(tan1, vertexCount * sizeof(Vector3) * 2);

        for (long a = 0; a < triangleCount; a++)
        {
            long i1 = triangle[3 * a + 0];
            long i2 = triangle[3 * a + 1];
            long i3 = triangle[3 * a + 2];

            const Vector3& v1 = vertex[i1];
            const Vector3& v2 = vertex[i2];
            const Vector3& v3 = vertex[i3];

            const Vector2& w1 = texcoord[i1];
            const Vector2& w2 = texcoord[i2];
            const Vector2& w3 = texcoord[i3];

            float x1 = v2.x - v1.x;
            float x2 = v3.x - v1.x;
            float y1 = v2.y - v1.y;
            float y2 = v3.y - v1.y;
            float z1 = v2.z - v1.z;
            float z2 = v3.z - v1.z;

            float s1 = w2.x - w1.x;
            float s2 = w3.x - w1.x;
            float t1 = w2.y - w1.y;
            float t2 = w3.y - w1.y;

            float r = 1.0F / (s1 * t2 - s2 * t1);
            Vector3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
                (t2 * z1 - t1 * z2) * r);
            Vector3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
                (s1 * z2 - s2 * z1) * r);

            tan1[i1] += sdir;
            tan1[i2] += sdir;
            tan1[i3] += sdir;

            tan2[i1] += tdir;
            tan2[i2] += tdir;
            tan2[i3] += tdir;
        }

        for (long a = 0; a < vertexCount; a++)
        {
            const Vector3& n = normal[a];
            const Vector3& t = tan1[a];

            // Gram-Schmidt orthogonalize
            tangent[a] = glm::normalize(t - n * glm::dot(n, t));
        }

        delete[] tan1;
    }

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
                const float* colors = nullptr;
                const float* tangents = nullptr;
                const float* texcoords0 = nullptr;
                const float* texcoords1 = nullptr;
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
                }

                // 访问顶点色数据
                if (primitive.attributes.find("COLOR") != primitive.attributes.end())
                {
                    int accessorIdx = primitive.attributes.at("COLOR");
                    const tinygltf::Accessor& accessor = model.accessors[accessorIdx];
                    const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                    const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                    if (accessor.type != TINYGLTF_TYPE_VEC3)
                    {
                        LOG_WARN("mesh {} normal data type error", path);
                    }
                    else
                    {
                        colors = reinterpret_cast<const float*>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));

                    }
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
                    }
                }

                // 访问纹理坐标数据
                if (primitive.attributes.find("TEXCOORD_1") != primitive.attributes.end())
                {
                    int accessorIdx = primitive.attributes.at("TEXCOORD_1");
                    const tinygltf::Accessor& accessor = model.accessors[accessorIdx];
                    const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                    const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                    if (accessor.type != TINYGLTF_TYPE_VEC2)
                    {
                        LOG_WARN("mesh {} texture coord 1 data type error", path);
                        continue;
                    }
                    else
                    {
                        texcoords1 = reinterpret_cast<const float*>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
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

                        const float* jointWeights = reinterpret_cast<const float*>(&(weightsBuffer.data[weightsAccessor.byteOffset + weightsBufferView.byteOffset]));
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

                RUSH_ASSERT(positions);
                int buffIndex = 0;
                vlayouts[buffIndex].attributeCount = 1;
                vlayouts[buffIndex].stride = 3 * sizeof(float);
                vlayouts[buffIndex].attributes[0].format = VertexFormat::Float32x3;
                vlayouts[buffIndex].attributes[0].offset = 0;
                vlayouts[buffIndex].attributes[0].location = VertexSementic::V_POSITION;
                ++buffIndex;

                if (colors)
                {
                    vlayouts[buffIndex].attributeCount = 1;
                    vlayouts[buffIndex].stride = 3 * sizeof(float);
                    vlayouts[buffIndex].attributes[0].format = VertexFormat::Float32x3;
                    vlayouts[buffIndex].attributes[0].offset = 0;
                    vlayouts[buffIndex].attributes[0].location = VertexSementic::V_COLOR;
                    ++buffIndex;
                }

                if (normals)
                {
                    vlayouts[buffIndex].attributeCount = 1;
                    vlayouts[buffIndex].stride = 3 * sizeof(float);
                    vlayouts[buffIndex].attributes[0].format = VertexFormat::Float32x3;
                    vlayouts[buffIndex].attributes[0].offset = 0;
                    vlayouts[buffIndex].attributes[0].location = VertexSementic::V_NORMAL;
                    ++buffIndex;
                }

                bool computeTangents = false;
                if (!tangents && texcoords0)
                {
                    tangents = new float[numVertices * 3];
                    CalculateTangentArray(numVertices, (const Vector3*)positions, (const Vector3*)normals, (const Vector2*)texcoords0, numIndices / 3, indices, (Vector3*)tangents);
                    computeTangents = true;
                }

                if (tangents)
                {
                    vlayouts[buffIndex].attributeCount = 1;
                    vlayouts[buffIndex].stride = 3 * sizeof(float);
                    vlayouts[buffIndex].attributes[0].format = VertexFormat::Float32x3;
                    vlayouts[buffIndex].attributes[0].offset = 0;
                    vlayouts[buffIndex].attributes[0].location = VertexSementic::V_TANGENT;
                    ++buffIndex;
                }

                if (texcoords0)
                {
                    vlayouts[buffIndex].attributeCount = 1;
                    vlayouts[buffIndex].stride = 2 * sizeof(float);
                    vlayouts[buffIndex].attributes[0].format = VertexFormat::Float32x2;
                    vlayouts[buffIndex].attributes[0].offset = 0;
                    vlayouts[buffIndex].attributes[0].location = VertexSementic::V_TEXCOORD0;
                    ++buffIndex;
                }

                if (texcoords1)
                {
                    vlayouts[buffIndex].attributeCount = 1;
                    vlayouts[buffIndex].stride = 2 * sizeof(float);
                    vlayouts[buffIndex].attributes[0].format = VertexFormat::Float32x2;
                    vlayouts[buffIndex].attributes[0].offset = 0;
                    vlayouts[buffIndex].attributes[0].location = VertexSementic::V_TEXCOORD1;
                    ++buffIndex;
                }

                if (jointIndices && jointWeights)
                {
                    vlayouts[buffIndex].attributeCount = 1;
                    vlayouts[buffIndex].stride = 4 * sizeof(uint16_t);
                    vlayouts[buffIndex].attributes[0].format = VertexFormat::Uint16x4;
                    vlayouts[buffIndex].attributes[0].offset = 0;
                    vlayouts[buffIndex].attributes[0].location = VertexSementic::V_JOINT;
                    ++buffIndex;

                    vlayouts[buffIndex].attributeCount = 1;
                    vlayouts[buffIndex].stride = 4 * sizeof(float);
                    vlayouts[buffIndex].attributes[0].format = VertexFormat::Float32x4;
                    vlayouts[buffIndex].attributes[0].offset = 0;
                    vlayouts[buffIndex].attributes[0].location = VertexSementic::V_WEIGHT;
                    ++buffIndex;
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
                if (texcoords1)
                {
                    geo->UpdateVertexBuffer(buffIndex++, texcoords1, numVertices * sizeof(float) * 2);
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

                if (computeTangents)
                {
                    delete[] tangents;
                }
            }

        }

        if (model.skins.size() > 0)
        {
            // Assuming there is only one skin in the model for simplicity
            const tinygltf::Skin& skin = model.skins[0];
            for (int jointIndex : skin.joints) {
                const tinygltf::Node& jointNode = model.nodes[jointIndex];
                // Access jointNode for bone information
            }
        }


        return true;
    }

}