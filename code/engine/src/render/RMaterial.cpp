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
#include "Core/json.h"

namespace rush
{

    struct MaterialParse
    {
        String type;
        String shading_model;
        String blend_mode;
        String vs;
        String fs;
        bool depth_write = true;
        bool depth_test = true;
        String front_face = "cw"; // cw
        String cull_mode = "back"; // "front" "none"
        bool has_color = false;
        bool has_normal = true;
        bool has_tangent = false;
        bool has_texcoord0 = true;
        bool has_texcoord1 = false;
        bool is_skined = false;
        String code;

        struct Binding
        {
            uint32_t binding;
            String type;
            String target;
            String path;
            String address = "repeat";
            String mag = "nearest";
            String min = "nearest";
            String mip = "nearest";
            RUSH_DEFINE_PROPERTIES_NON_INTRUSIVE_WITH_DEFAULT(Binding, binding, type, target, path, address, mag, min, mip);
        };
        DArray<Binding> uniforms;

        RUSH_DEFINE_PROPERTIES_NON_INTRUSIVE_WITH_DEFAULT(MaterialParse, type, shading_model, blend_mode, vs, fs,
                            depth_write, depth_test, front_face, cull_mode, uniforms,
                            has_color, has_normal, has_texcoord0, has_texcoord1, is_skined, code);
    };


    Map<uint64_t, wgpu::RenderPipeline> RMaterial::s_PipelineCache;

    bool RMaterial::Load(const StringView& path)
    {
        m_Path = path;

        auto stream = BundleManager::instance().Get(path);
        if (stream->IsEmpty())
        {
            LOG_ERROR("Cannot load material {}", path.data());
            return false;
        }

        MaterialParse matData = Json::parse((const char*)stream->GetData(), nullptr, false);

        List<String> defines;

        if (matData.type == "surface")
        {
            m_Type = MaterialType::Surface;
        }
        else if (matData.type == "post_process")
        {
            m_Type = MaterialType::PostProcess;
        }

        if (matData.shading_model == "default_lit")
        {
            m_ShadingModel = ShadingModel::DefaultLit;
            defines.push_back("SHADING_MODEL=SHADING_MODEL_DEFAULT_LIT");
        }
        else if (matData.shading_model == "unlit")
        {
            m_ShadingModel = ShadingModel::Unlit;
            defines.push_back("SHADING_MODEL=SHADING_MODEL_UNLIT");
        }

        if (matData.blend_mode == "opaque")
        {
            blendMode = BlendMode::Opaque;
            defines.push_back("BLEND_MODE=BLEND_MODE_OPAQUE");
        }
        else if (matData.blend_mode == "transparent")
        {
            blendMode = BlendMode::Transparent;
            defines.push_back("BLEND_MODE=BLEND_MODE_TRANSPARENT");
        }
        else if (matData.blend_mode == "masked")
        {
            blendMode = BlendMode::Masked;
            defines.push_back("BLEND_MODE=BLEND_MODE_MASKED");
        }

        // vertex flags
        if (matData.has_color)
        {
            defines.push_back("HAS_COLOR");
        }
        else if (matData.has_normal)
        {
            defines.push_back("HAS_NORMAL");
        }
        else if (matData.has_tangent)
        {
            defines.push_back("HAS_TANGENTS");
        }
        else if (matData.has_texcoord0)
        {
            defines.push_back("HAS_TEXCOORD0");
        }
        else if (matData.has_texcoord0)
        {
            defines.push_back("HAS_TEXCOORD1");
        }
        else if (matData.is_skined)
        {
            defines.push_back("IS_SKINNED");
        }

        depthTest = matData.depth_test;
        depthWrite = matData.depth_write;

        if (matData.front_face == "cw")
        {
            frontFace = FrontFace::CW;
        }
        else if (matData.front_face == "ccw")
        {
            frontFace = FrontFace::CCW;
        }

        if (matData.cull_mode == "none")
        {
            cullMode = CullMode::None;
        }
        else if (matData.cull_mode == "front")
        {
            cullMode = CullMode::Front;
        }
        else if (cullMode == CullMode::Back)
        {
            cullMode = CullMode::Back;
        }

        RUSH_ASSERT(matData.vs != "" && matData.fs != "");
        AssetsManager::instance().LoadOrCompileShader(matData.vs, defines, "", [&](AssetLoadResult result, Ref<RShader> shader, void* param) {
            m_VertexShader = shader;
        }, nullptr);

        AssetsManager::instance().LoadOrCompileShader(matData.fs, defines, matData.code, [&](AssetLoadResult result, Ref<RShader> shader, void* param) {
            m_FragmentShader = shader;
        }, nullptr);

        if (m_VertexShader == nullptr || m_FragmentShader == nullptr)
        {
            return false;
        }

        if (matData.uniforms.size() > 0)
        {
            for (auto& uniform : matData.uniforms)
            {
                if (uniform.type == "texture")
                {
                    auto& info = m_BindInfos.emplace_back();
                    info.binding = uniform.binding;
                    info.type = BindingType::Texture;
                    if (uniform.target != "")
                    {
                        info.target = uniform.target;
                    }
                    else if (uniform.path != "")
                    {
                        info.path = uniform.path;
                    }
                }
                else if (uniform.type == "sampler")
                {
                    auto& info = m_BindInfos.emplace_back();
                    info.binding = uniform.binding;
                    info.type = BindingType::Sampler;
                    info.address = uniform.address;
                    info.mag = uniform.mag;
                    info.min = uniform.min;
                    info.mip = uniform.mip;
                }
                else if (uniform.type == "uniform")
                {
                    auto& info = m_BindInfos.emplace_back();
                    info.binding = uniform.binding;
                    info.type = BindingType::Uniform;
                }
                else if (uniform.type == "storage")
                {
                    auto& info = m_BindInfos.emplace_back();
                    info.binding = uniform.binding;
                    info.type = BindingType::Storage;
                }
            }
        }


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

            if (material->GetType() == MaterialType::Surface)
            {
                bindingGroupLayouts.push_back(renderer->GetFrameDataGroup()->GetBindLayoutHandle());
                bindingGroupLayouts.push_back(renderer->GetLightDataGroup()->GetBindLayoutHandle());
            }

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
                        AddressMode address;
                        if (info.address == "repeat")
                        {
                            address = AddressMode::Repeat;
                        }
                        else if (info.address == "mirror")
                        {
                            address = AddressMode::MirrorRepeat;
                        }
                        else if (info.address == "edge")
                        {
                            address = AddressMode::ClampToEdge;
                        }

                        FilterMode mag, min;
                        MipmapFilterMode mip;
                        if (info.mag == "linear")
                        {
                            mag = FilterMode::Linear;
                        }
                        else if (info.mag == "nearest")
                        {
                            mag = FilterMode::Nearest;
                        }
                        if (info.min == "linear")
                        {
                            min = FilterMode::Linear;
                        }
                        else if (info.min == "nearest")
                        {
                            min = FilterMode::Nearest;
                        }
                        if (info.mip == "linear")
                        {
                            mip = MipmapFilterMode::Linear;
                        }
                        else if (info.mip == "nearest")
                        {
                            mip = MipmapFilterMode::Nearest;
                        }

                        wgpu::SamplerDescriptor desc = {};
                        desc.addressModeU = address;
                        desc.addressModeV = address;
                        desc.addressModeW = address;
                        desc.magFilter = mag;
                        desc.minFilter = min;
                        desc.mipmapFilter = mip;
                        desc.maxAnisotropy = 1;
                        auto sampler = RDevice::instance().GetDevice().CreateSampler(&desc);
                        material->m_BindGroup->AddBinding(info.binding, ShaderStage::Vertex | ShaderStage::Fragment, sampler, wgpu::SamplerBindingType::Filtering);
                    }
                }

                material->m_BindGroup->Create(lyoutlabel.c_str());
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