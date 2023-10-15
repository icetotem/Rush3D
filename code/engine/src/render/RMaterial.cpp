#include "stdafx.h"
#include "render/RMaterial.h"
#include "AssetManager.h"
#include "core/Common.h"
#include "render/RGeometry.h"
#include "render/RShader.h"
#include "BundleManager.h"
#include "render/RShader.h"
#include "render/RDevice.h"
#include "render/RBindGroup.h"
#include <libconfig/libconfig.h>

namespace rush
{

    Map<uint64_t, wgpu::RenderPipeline> RMaterial::s_PipelineCache;

    bool RMaterial::Load(const StringView& path)
    {
        m_Path = path;
        m_Hash = 0;
        List<String> defines;

        auto stream = BundleManager::instance().Get(path);
        if (stream->IsEmpty())
        {
            LOG_ERROR("Cannot load material {}", path.data());
            return false;
        }

        config_t cfg;
        config_init(&cfg);

        if (CONFIG_TRUE != config_read_string(&cfg, (const char*)stream->GetData()))
        {
            LOG_ERROR("material syntex error {}", path.data());
            return false;
        }

        const char* tempStr = nullptr;
        if (CONFIG_TRUE == config_lookup_string(&cfg, "material.type", &tempStr))
        {
            if (String(tempStr) == "surface")
            {
                m_Type = MaterialType::Surface;
            }
            else if (String(tempStr) == "post_process")
            {
                m_Type = MaterialType::PostProcess;
            }
        }

        if (CONFIG_TRUE == config_lookup_string(&cfg, "material.shading_model", &tempStr))
        {
            if (String(tempStr) == "default_lit")
            {
                m_ShadingModel = ShadingModel::DefaultLit;
                defines.push_back("SHADING_MODEL=SHADING_MODEL_DEFAULT_LIT");
            }
            else if (String(tempStr) == "unlit")
            {
                m_ShadingModel = ShadingModel::Unlit;
                defines.push_back("SHADING_MODEL=SHADING_MODEL_UNLIT");
            }
        }

        if (CONFIG_TRUE == config_lookup_string(&cfg, "material.blend_mode", &tempStr))
        {
            if (String(tempStr) == "opaque")
            {
                blendMode = BlendMode::Opaque;
                defines.push_back("BLEND_MODE=BLEND_MODE_OPAQUE");
            }
            else if (String(tempStr) == "transparent")
            {
                blendMode = BlendMode::Transparent;
                defines.push_back("BLEND_MODE=BLEND_MODE_TRANSPARENT");
            }
            else if (String(tempStr) == "masked")
            {
                blendMode = BlendMode::Masked;
                defines.push_back("BLEND_MODE=BLEND_MODE_MASKED");
            }
        }

        int tempBool = 0;
        if (CONFIG_TRUE == config_lookup_bool(&cfg, "material.depth_test", &tempBool))
        {
            depthTest = (bool)tempBool;
        }

        if (CONFIG_TRUE == config_lookup_bool(&cfg, "material.depth_write", &tempBool))
        {
            depthWrite = (bool)tempBool;
        }

        if (CONFIG_TRUE == config_lookup_string(&cfg, "material.front_face", &tempStr))
        {
            if (String(tempStr) == "cw")
            {
                frontFace = FrontFace::CW;
            }
            else if (String(tempStr) == "ccw")
            {
                frontFace = FrontFace::CCW;
            }
        }

        if (CONFIG_TRUE == config_lookup_string(&cfg, "material.cull_mode", &tempStr))
        {
            if (String(tempStr) == "none")
            {
                cullMode = CullMode::None;
            }
            else if (String(tempStr) == "front")
            {
                cullMode = CullMode::Front;
            }
            else if (String(tempStr) == "back")
            {
                cullMode = CullMode::Back;
            }
        }

        if (CONFIG_TRUE == config_lookup_bool(&cfg, "material.has_color", &tempBool) && tempBool)
        {
            defines.push_back("HAS_COLOR");
        }
        if (CONFIG_TRUE == config_lookup_bool(&cfg, "material.has_normal", &tempBool) && tempBool)
        {
            defines.push_back("HAS_NORMAL");
        }
        if (CONFIG_TRUE == config_lookup_bool(&cfg, "material.has_tangent", &tempBool) && tempBool)
        {
            defines.push_back("HAS_TANGENTS");
        }
        if (CONFIG_TRUE == config_lookup_bool(&cfg, "material.has_texcoord0", &tempBool) && tempBool)
        {
            defines.push_back("HAS_TEXCOORD0");
        }
        if (CONFIG_TRUE == config_lookup_bool(&cfg, "material.has_texcoord1", &tempBool) && tempBool)
        {
            defines.push_back("HAS_TEXCOORD1");
        }
        if (CONFIG_TRUE == config_lookup_bool(&cfg, "material.is_skined", &tempBool) && tempBool)
        {
            defines.push_back("IS_SKINNED");
        }

        String customCode;
        if (CONFIG_TRUE == config_lookup_string(&cfg, "material.code", &tempStr))
        {
            customCode = tempStr;
        }

        auto globalBindGroups = config_lookup(&cfg, "material.global_bind_groups");
        if (globalBindGroups)
        {
            int count = config_setting_length(globalBindGroups);
            for (int i = 0; i < count; ++i)
            {
                auto bg = config_setting_get_elem(globalBindGroups, i);
                m_GlobalBindGroups.push_back(bg->value.sval);
            }
        }


        String uniformCode;
        auto uniforms = config_lookup(&cfg, "material.uniforms");
        if (uniforms)
        {
            int count = config_setting_length(uniforms);
            for (int i = 0; i < count; ++i)
            {
                auto uniform = config_setting_get_elem(uniforms, i);
                RUSH_ASSERT(uniform);

                auto& info = m_BindInfos.emplace_back();
                int binding = 0;
                if (CONFIG_TRUE != config_setting_lookup_int(uniform, "binding", &binding))
                {
                    LOG_ERROR("Material uniform must define a binding");
                    return false;
                }
                info.binding = binding;
                hash_combine(m_BindGroupHash, binding);

                const char* type = nullptr;
                if (CONFIG_TRUE != config_setting_lookup_string(uniform, "type", &type))
                {
                    LOG_ERROR("Material uniform must define a type");
                    return false;
                }

                const char* name = nullptr;
                if (CONFIG_TRUE == config_setting_lookup_string(uniform, "name", &name))
                {
                    info.name = name;
                }
                hash_combine(m_BindGroupHash, info.name);

                if (String(type) == "texture" || String(type) == "texture_cube" || String(type) == "texture_3d")
                {
                    info.type = BindingType::Texture;
                    hash_combine(m_BindGroupHash, info.type);

                    if (String(type) == "texture")
                    {
                        info.dim = TextureViewDimension::e2D;
                    }
                    else if (String(type) == "texture_cube")
                    {
                        info.dim = TextureViewDimension::Cube;
                    }
                    else if (String(type) == "texture_3d")
                    {
                        info.dim = TextureViewDimension::e3D;
                    }

                    const char* target = nullptr;
                    const char* path = nullptr;
                    if (CONFIG_TRUE == config_setting_lookup_string(uniform, "target", &target))
                    {
                        info.target = target;
                        hash_combine(m_BindGroupHash, target);

                        const char* sampleType;
                        if (CONFIG_TRUE == config_setting_lookup_string(uniform, "sample_type", &sampleType))
                        {
                            if (String(sampleType) == "float")
                            {
                                info.textureSampleType = TextureSampleType::Float;
                            }
                            else if (String(sampleType) == "unfilterable")
                            {
                                info.textureSampleType = TextureSampleType::UnfilterableFloat;
                            }
                            else if (String(sampleType) == "depth")
                            {
                                info.textureSampleType = TextureSampleType::Depth;
                            }
                            else if (String(sampleType) == "sint")
                            {
                                info.textureSampleType = TextureSampleType::Sint;
                            }
                            else if (String(sampleType) == "uint")
                            {
                                info.textureSampleType = TextureSampleType::Uint;
                            }
                            else
                            {
                                LOG_ERROR("Unknown sample type {}", sampleType);
                            }
                            hash_combine(m_BindGroupHash, sampleType);
                        }
                    }
                    else if (CONFIG_TRUE == config_setting_lookup_string(uniform, "path", &path))
                    {
                        info.path = path;
                        hash_combine(m_BindGroupHash, path);
                    }
                    else
                    {
                        LOG_ERROR("Material texture uniform must define a target or path");
                        return false;
                    }
                }
                else if (String(type) == "sampler")
                {
                    info.type = BindingType::Sampler;
                    hash_combine(m_BindGroupHash, info.type);

                    const char* address = nullptr;
                    if (CONFIG_TRUE == config_setting_lookup_string(uniform, "address", &address))
                    {
                        if (String(address) == "repeat")
                        {
                            info.address = AddressMode::Repeat;
                        }
                        else if (String(address) == "mirror")
                        {
                            info.address = AddressMode::MirrorRepeat;
                        }
                        else if (String(address) == "edge")
                        {
                            info.address = AddressMode::ClampToEdge;
                        }
                        hash_combine(m_BindGroupHash, info.address);
                    }
                    const char* mag = nullptr;
                    if (CONFIG_TRUE == config_setting_lookup_string(uniform, "mag", &mag))
                    {
                        if (String(mag) == "linear")
                        {
                            info.mag = FilterMode::Linear;
                        }
                        else if (String(mag) == "nearest")
                        {
                            info.mag = FilterMode::Nearest;
                        }
                        hash_combine(m_BindGroupHash, info.mag);
                    }
                    const char* min = nullptr;
                    if (CONFIG_TRUE == config_setting_lookup_string(uniform, "min", &min))
                    {
                        if (String(min) == "linear")
                        {
                            info.min = FilterMode::Linear;
                        }
                        else if (String(min) == "nearest")
                        {
                            info.min = FilterMode::Nearest;
                        }
                        hash_combine(m_BindGroupHash, info.min);
                    }
                    const char* mip = nullptr;
                    if (CONFIG_TRUE == config_setting_lookup_string(uniform, "mip", &mip))
                    {
                        if (String(mip) == "linear")
                        {
                            info.mip = MipmapFilterMode::Linear;
                        }
                        else if (String(mip) == "nearest")
                        {
                            info.mip = MipmapFilterMode::Nearest;
                        }
                        hash_combine(m_BindGroupHash, info.mip);
                    }

                    const char* sampleBindType;
                    if (CONFIG_TRUE == config_setting_lookup_string(uniform, "binding_type", &sampleBindType))
                    {
                        if (String(sampleBindType) == "filtering")
                        {
                            info.samplerBindingType = SamplerBindingType::Filtering;
                        }
                        else if (String(sampleBindType) == "non_filtering")
                        {
                            info.samplerBindingType = SamplerBindingType::NonFiltering;
                        }
                        else if (String(sampleBindType) == "comparison")
                        {
                            info.samplerBindingType = SamplerBindingType::Comparison;
                        }
                        else
                        {
                            LOG_ERROR("Unknown sampler binding type {}", sampleBindType);
                        }
                        hash_combine(m_BindGroupHash, sampleBindType);
                    }
                }
                else if (String(type) == "uniform")
                {
                    info.type = BindingType::Uniform;
                    hash_combine(m_BindGroupHash, info.type);
                    int size = 0;
                    if (CONFIG_TRUE == config_setting_lookup_int(uniform, "size", &size))
                    {
                        info.size = size;
                        hash_combine(m_BindGroupHash, info.size);
                    }
                }
                else if (String(type) == "storage")
                {
                    info.type = BindingType::Storage;
                    hash_combine(m_BindGroupHash, info.type);
                }

                char uniformLine[128];
                if (info.type == BindingType::Texture && info.name != "")
                {
                    String tex;
                    if (info.dim == TextureViewDimension::e2D)
                        tex = "texture2D";
                    else if (info.dim == TextureViewDimension::e3D)
                        tex = "texture2D";
                    else if (info.dim == TextureViewDimension::Cube)
                        tex = "textureCube";

                    sprintf(uniformLine, "layout(set = 1, binding = %d) uniform %s %s;\n", info.binding, tex.c_str(), info.name.c_str());
                    uniformCode += uniformLine;
                }
                else if (info.type == BindingType::Sampler && info.name != "")
                {
                    sprintf(uniformLine, "layout(set = 1, binding = %d) uniform sampler %s;\n", info.binding, info.name.c_str());
                    uniformCode += uniformLine;
                }
//                 else if (info.type == BindingType::Uniform && info.name != "")
//                 {
//                     sprintf(uniformLine, "layout(set = 2, binding = %d) uniform sampler %s;\n", info.binding, info.name.c_str());
//                 }
            }
        }

        if (CONFIG_TRUE == config_lookup_string(&cfg, "material.vs", &tempStr))
        {
            AssetsManager::instance().LoadOrCompileShader(tempStr, defines, "", "", [&](AssetLoadResult result, Ref<RShader> shader, void* param) {
                m_VertexShader = shader;
            }, nullptr);
        }

        if (CONFIG_TRUE == config_lookup_string(&cfg, "material.fs", &tempStr))
        {
            if (m_Type == MaterialType::Surface)
            {
                AssetsManager::instance().LoadOrCompileShader(tempStr, defines, customCode, uniformCode, [&](AssetLoadResult result, Ref<RShader> shader, void* param) {
                    m_FragmentShader = shader;
                }, nullptr);
            }
            else
            {
                AssetsManager::instance().LoadOrCompileShader(tempStr, defines, "", "", [&](AssetLoadResult result, Ref<RShader> shader, void* param) {
                    m_FragmentShader = shader;
                }, nullptr);
            }
        }

        if (m_VertexShader == nullptr || m_FragmentShader == nullptr)
        {
            return false;
        }

        config_destroy(&cfg);

        // hash
        hash_combine(m_ShaderHash, m_Type);
        hash_combine(m_ShaderHash, m_ShadingModel);
        hash_combine(m_ShaderHash, m_VertexShader->GetHash());
        hash_combine(m_ShaderHash, m_FragmentShader->GetHash());
        for (const auto& define : defines)
        {
            hash_combine(m_ShaderHash, define);
        }
        hash_combine(m_ShaderHash, customCode);

        hash_combine(m_StateHash, cullMode);
        hash_combine(m_StateHash, writeMask);
        hash_combine(m_StateHash, depthTest);
        hash_combine(m_StateHash, depthWrite);
        hash_combine(m_StateHash, depthCompare);
        hash_combine(m_StateHash, stencilTest);
        hash_combine(m_StateHash, stencilWrite);
        hash_combine(m_StateHash, opColor);
        hash_combine(m_StateHash, srcColor);
        hash_combine(m_StateHash, dstColor);
        hash_combine(m_StateHash, opAlpha);
        hash_combine(m_StateHash, srcAlpha);
        hash_combine(m_StateHash, dstAlpha);

        hash_combine(m_Hash, m_BindGroupHash);
        hash_combine(m_Hash, m_ShaderHash);
        hash_combine(m_Hash, m_StateHash);

        return true;
    }

    void RMaterial::UpdateFrameData(const FrameData& data)
    {
        
    }

    const wgpu::RenderPipeline RMaterial::GetPipeline(Renderer* renderer, Ref<RGeometry> geometry, Ref<RMaterial> material, const FrameBuffer& outputBuffers)
    {
        RUSH_ASSERT(geometry != nullptr && material != nullptr);

        uint64_t h(0);
        hash_val(h, geometry->GetLayoutHash(), material->GetHash());
        auto iter = s_PipelineCache.find(h);
        if (iter != s_PipelineCache.end())
        {
            return iter->second;
        }
        else
        {
            wgpu::PipelineLayoutDescriptor plLayoutDesc = {};
            auto lyoutlabel = material->GetPath() + String("_PipelineLayout_") + std::to_string(material->GetHash());
            plLayoutDesc.label = lyoutlabel.c_str();

            DArray<wgpu::BindGroupLayout> bindingGroupLayouts;

            // add frame data group
            bindingGroupLayouts.push_back(renderer->GetFrameDataGroup()->GetBindLayoutHandle());

            if (material->m_BindInfos.size() > 0)
            {
                if (material->m_BindGroup == nullptr)
                    material->m_BindGroup = CreateRef<RBindGroup>();

                for (const auto& info : material->m_BindInfos)
                {
                    if (info.type == BindingType::Texture)
                    {
                        if (info.target.has_value())
                        {
                            auto tex = renderer->GetFGTexture(info.target.value());
                            RUSH_ASSERT(tex);
                            TextureSampleType samplerType = TextureSampleType::Float;
                            if (info.textureSampleType.has_value())
                            {
                                samplerType = info.textureSampleType.value();
                            }
                            char temp[128];
                            sprintf(temp, "%s_TextureView_%d_%s", material->GetPath().c_str(), info.binding, info.name.c_str());
                            material->m_BindGroup->AddBinding(info.binding, ShaderStage::Vertex | ShaderStage::Fragment, tex, samplerType, info.dim.value(), temp);
                        }
                        else if (info.path.has_value())
                        {
                            char temp[128];
                            sprintf(temp, "%s_TextureView_%d_%s", material->GetPath().c_str(), info.binding, info.name.c_str());
                            AssetsManager::instance().LoadTexture(info.path.value(), [&](AssetLoadResult result, Ref<RTexture> tex, void* param){
                                material->m_BindGroup->AddBinding(info.binding, ShaderStage::Vertex | ShaderStage::Fragment, tex, TextureSampleType::Float, info.dim.value(), temp);
                            });
                        }
                    }
                    else if (info.type == BindingType::Sampler)
                    {
                        wgpu::SamplerDescriptor desc = {};
                        if (info.address.has_value())
                        {
                            desc.addressModeU = info.address.value();
                            desc.addressModeV = info.address.value();
                            desc.addressModeW = info.address.value();
                        }
                        if (info.mag.has_value())
                        {
                            desc.magFilter = info.mag.value();
                        }
                        if (info.min.has_value())
                        {
                            desc.minFilter = info.min.value();
                        }
                        if (info.mip.has_value())
                        {
                            desc.mipmapFilter = info.mip.value();
                        }
     
                        desc.maxAnisotropy = 1;
                        auto sampler = RDevice::instance().GetDevice().CreateSampler(&desc);
                        wgpu::SamplerBindingType type = wgpu::SamplerBindingType::Filtering;
                        if (info.samplerBindingType.has_value())
                        {
                            type = info.samplerBindingType.value();
                        }
                        material->m_BindGroup->AddBinding(info.binding, ShaderStage::Vertex | ShaderStage::Fragment, sampler, type);
                    }
                    else if (info.type == BindingType::Uniform)
                    {
                        material->m_UniformBuffer = CreateRef<RUniformBuffer>(info.size.value());
                        uint32_t mode = 0;
                        material->m_UniformBuffer->UpdateData(&mode, sizeof(uint32_t));
                        material->m_BindGroup->AddBinding(info.binding, ShaderStage::Vertex | ShaderStage::Fragment, material->m_UniformBuffer, wgpu::BufferBindingType::Uniform);
                    }
                }

                auto bindGroupLabel = material->GetPath() + String("_BindGroup_") + std::to_string(material->GetHash());
                material->m_BindGroup->Create(bindGroupLabel.c_str());
                bindingGroupLayouts.push_back(material->m_BindGroup->GetBindLayoutHandle());
            }

            // add transform and instance data group
            if (material->GetType() == MaterialType::Surface)
            {
                bindingGroupLayouts.push_back(renderer->GetTransformDataGroup()->GetBindLayoutHandle());
                bindingGroupLayouts.push_back(renderer->GetInstanceDataGroup()->GetBindLayoutHandle());
            }
            else if (material->GetType() == MaterialType::PostProcess)
            {
                for (auto bg : material->m_GlobalBindGroups)
                {
                    if (bg == "light_data")
                    {
                        bindingGroupLayouts.push_back(renderer->GetLightDataGroup()->GetBindLayoutHandle());
                    }
                    else if (bg == "transform_data")
                    {
                        bindingGroupLayouts.push_back(renderer->GetTransformDataGroup()->GetBindLayoutHandle());
                    }
                    else if (bg == "instance_data")
                    {
                        bindingGroupLayouts.push_back(renderer->GetInstanceDataGroup()->GetBindLayoutHandle());
                    }
                }
            }

            plLayoutDesc.bindGroupLayoutCount = bindingGroupLayouts.size();
            plLayoutDesc.bindGroupLayouts = bindingGroupLayouts.data();

            wgpu::RenderPipelineDescriptor descriptor = {};
            auto pllabel = material->GetPath() + String("_Pipeline_") + std::to_string(material->GetHash());
            descriptor.label = pllabel.c_str();
            descriptor.layout = RDevice::instance().GetDevice().CreatePipelineLayout(&plLayoutDesc);

            // Setup vertex state.
            std::array<wgpu::VertexBufferLayout, kMaxVertexBuffers> cBuffers;
            std::array<wgpu::VertexAttribute, kMaxVertexAttributes> cAttributes;
            {
                wgpu::VertexState* vertex = &descriptor.vertex;
                vertex->module = material->GetVertexShader()->GetModule();
                vertex->entryPoint = "main";

                uint32_t i = 0;
                int attr = 0;
                for (;i < geometry->GetVBCount();)
                {
                    auto& layout = geometry->GetLayouts()[i];
                    cBuffers[i].stepMode = wgpu::VertexStepMode::Vertex;
                    cBuffers[i].arrayStride = layout.stride;
                    cBuffers[i].attributeCount = layout.attributeCount;
                    cBuffers[i].attributes = &cAttributes[attr];
                    ++i;

                    for (uint32_t j = 0; j < layout.attributeCount; ++j)
                    {
                        cAttributes[attr].format = layout.attributes[j].format;
                        cAttributes[attr].offset = layout.attributes[j].offset;
                        cAttributes[attr].shaderLocation = layout.attributes[j].location;
                        ++attr;
                    }
                }

                vertex->bufferCount = geometry->GetVBCount();
                vertex->buffers = cBuffers.data();
            }

            // Setup fragment state
            wgpu::FragmentState cFragment = {};
            wgpu::ColorTargetState cTargets[kMaxFrameBufferss];
            wgpu::BlendState cBlendState[kMaxFrameBufferss];
            {
                cFragment.module = material->GetFragmentShader()->GetModule();
                cFragment.entryPoint = "main";
                descriptor.fragment = &cFragment;
                cFragment.targetCount = outputBuffers.colorAttachment.size();
                cFragment.targets = cTargets;
                for (int i = 0; i < cFragment.targetCount; ++i)
                {
                    cTargets[i].format = outputBuffers.colorAttachment[i].format;
                    cTargets[i].writeMask = (wgpu::ColorWriteMask)material->writeMask;

                    if (material->GetBlendMode() == BlendMode::Transparent)
                    {
                        cTargets[i].blend = cBlendState;
                        cBlendState[i].color.srcFactor = material->srcColor;
                        cBlendState[i].color.dstFactor = material->dstColor;
                        cBlendState[i].color.operation = material->opColor;
                        cBlendState[i].alpha.srcFactor = material->srcAlpha;
                        cBlendState[i].alpha.dstFactor = material->dstAlpha;
                        cBlendState[i].alpha.operation = material->opAlpha;
                    }
                }
            }

            // Set the defaults for the primitive state
            {
                wgpu::PrimitiveState* primitive = &descriptor.primitive;
                primitive->topology = geometry->GetPrimitiveType();
                primitive->frontFace = material->frontFace;
                primitive->cullMode = material->cullMode;
                primitive->stripIndexFormat = wgpu::IndexFormat::Undefined;
            }

            // Set the defaults for the depth-stencil state
            wgpu::DepthStencilState cDepthStencil = {};
            if (material->depthTest || material->depthWrite || material->stencilTest || material->stencilWrite)
            {
                descriptor.depthStencil = &cDepthStencil;
                cDepthStencil.format = outputBuffers.depthStencilFormat.has_value() ? outputBuffers.depthStencilFormat.value() : TextureFormat::Depth24PlusStencil8;
                cDepthStencil.depthWriteEnabled = material->depthWrite;
                if (material->depthTest)
                    cDepthStencil.depthCompare = material->depthCompare;
                else
                    cDepthStencil.depthCompare = wgpu::CompareFunction::Always;
                cDepthStencil.depthBias = 0;
                cDepthStencil.depthBiasSlopeScale = 0.0;
                cDepthStencil.depthBiasClamp = 0.0;

                wgpu::StencilFaceState stencilFace;
                stencilFace.compare = wgpu::CompareFunction::Always;
                stencilFace.failOp = wgpu::StencilOperation::Keep;
                stencilFace.depthFailOp = wgpu::StencilOperation::Keep;
                stencilFace.passOp = wgpu::StencilOperation::Keep;
                cDepthStencil.stencilBack = stencilFace;
                cDepthStencil.stencilFront = stencilFace;
                cDepthStencil.stencilReadMask = 0xff;
                cDepthStencil.stencilWriteMask = 0xff;
            }
            else
            {
                descriptor.depthStencil = nullptr;
            }

            auto pipeline = RDevice::instance().GetDevice().CreateRenderPipeline(&descriptor);
            s_PipelineCache.insert({ h, pipeline });
            return s_PipelineCache[h];
        }
    }
}