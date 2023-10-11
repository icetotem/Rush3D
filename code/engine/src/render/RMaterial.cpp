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
        else if (CONFIG_TRUE == config_lookup_bool(&cfg, "material.has_normal", &tempBool) && tempBool)
        {
            defines.push_back("HAS_NORMAL");
        }
        else if (CONFIG_TRUE == config_lookup_bool(&cfg, "material.has_tangent", &tempBool) && tempBool)
        {
            defines.push_back("HAS_TANGENTS");
        }
        else if (CONFIG_TRUE == config_lookup_bool(&cfg, "material.has_texcoord0", &tempBool) && tempBool)
        {
            defines.push_back("HAS_TEXCOORD0");
        }
        else if (CONFIG_TRUE == config_lookup_bool(&cfg, "material.has_texcoord1", &tempBool) && tempBool)
        {
            defines.push_back("HAS_TEXCOORD1");
        }
        else if (CONFIG_TRUE == config_lookup_bool(&cfg, "material.is_skined", &tempBool) && tempBool)
        {
            defines.push_back("IS_SKINNED");
        }

        String tempCode;
        if (CONFIG_TRUE == config_lookup_string(&cfg, "material.code", &tempStr))
        {
            tempCode = tempStr;
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
                const char* type = nullptr;
                if (CONFIG_TRUE != config_setting_lookup_string(uniform, "type", &type))
                {
                    LOG_ERROR("Material uniform must define a type");
                    return false;
                }

                const char* name = nullptr;
                if (CONFIG_TRUE != config_setting_lookup_string(uniform, "name", &name))
                {
                    LOG_ERROR("Material uniform must define a name");
                    return false;
                }
                info.name = name;

                if (String(type) == "texture")
                {
                    info.type = BindingType::Texture;
                    const char* target = nullptr;
                    const char* path = nullptr;
                    if (CONFIG_TRUE == config_setting_lookup_string(uniform, "target", &target))
                    {
                        info.target = target;
                    }
                    else if (CONFIG_TRUE == config_setting_lookup_string(uniform, "path", &path))
                    {
                        info.path = path;
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
                    }
                }
                else if (String(type) == "uniform")
                {
                    info.type = BindingType::Uniform;
                    int size = 0;
                    if (CONFIG_TRUE == config_setting_lookup_int(uniform, "size", &size))
                    {
                        info.size = size;
                    }
                }
                else if (String(type) == "storage")
                {
                    info.type = BindingType::Storage;
                }

                char uniformLine[128];
                if (info.type == BindingType::Texture)
                {
                    sprintf(uniformLine, "layout(set = 2, binding = %d) uniform texture2D %s;\n", info.binding, info.name.c_str());
                    uniformCode += uniformLine;
                }
                else if (info.type == BindingType::Sampler)
                {
                    sprintf(uniformLine, "layout(set = 2, binding = %d) uniform sampler %s;\n", info.binding, info.name.c_str());
                    uniformCode += uniformLine;
                }
//                 else if (info.type == BindingType::Uniform)
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
                AssetsManager::instance().LoadOrCompileShader(tempStr, defines, tempCode, uniformCode, [&](AssetLoadResult result, Ref<RShader> shader, void* param) {
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
        hash_combine(m_Hash, m_Type);
        hash_combine(m_Hash, m_VertexShader->GetHash());
        hash_combine(m_Hash, m_FragmentShader->GetHash());
        hash_combine(m_Hash, cullMode);
        hash_combine(m_Hash, writeMask);
        hash_combine(m_Hash, depthTest);
        hash_combine(m_Hash, depthWrite);

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

            bindingGroupLayouts.push_back(renderer->GetFrameDataGroup()->GetBindLayoutHandle());
            bindingGroupLayouts.push_back(renderer->GetTransformDataGroup()->GetBindLayoutHandle());

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
                            material->m_BindGroup->AddBinding(info.binding, ShaderStage::Vertex | ShaderStage::Fragment, tex, TextureSampleType::Float, TextureViewDimension::e2D);
                        }
                        else if (info.path.has_value())
                        {
                            AssetsManager::instance().LoadTexture(info.path.value(), [&](AssetLoadResult result, Ref<RTexture> tex, void* param){
                                material->m_BindGroup->AddBinding(info.binding, ShaderStage::Vertex | ShaderStage::Fragment, tex, TextureSampleType::Float, TextureViewDimension::e2D);
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
                        material->m_BindGroup->AddBinding(info.binding, ShaderStage::Vertex | ShaderStage::Fragment, sampler, wgpu::SamplerBindingType::Filtering);
                    }
                    else if (info.type == BindingType::Uniform)
                    {
                        material->m_UniformBuffer = CreateRef<RUniformBuffer>(info.size.value());
                        uint32_t mode = 0;
                        material->m_UniformBuffer->UpdateData(&mode, sizeof(uint32_t));
                        material->m_BindGroup->AddBinding(info.binding, ShaderStage::Vertex | ShaderStage::Fragment, material->m_UniformBuffer);
                    }
                }

                auto bindGroupLabel = material->GetPath() + String("_BindGroup_") + std::to_string(material->GetHash());
                material->m_BindGroup->Create(bindGroupLabel.c_str());
                bindingGroupLayouts.push_back(material->m_BindGroup->GetBindLayoutHandle());
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