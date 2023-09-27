#include "stdafx.h"
#include "render/RMaterial.h"
#include "AssetManager.h"
#include "core/Common.h"
#include "render/RGeometry.h"
#include "render/RShader.h"
#include "render/RPipeline.h"
#include "render/RUniform.h"
#include "BundleManager.h"
#include "render/RShader.h"
#include "render/RDevice.h"
#include "Core/json.h"

namespace rush
{

    struct MaterialParse
    {
        String type;
        String shading_model;
        String vs;
        String fs;
        bool depth_write = true;
        bool depth_test = true;

        struct Binding
        {
            uint32_t binding;
            String type;
            String target;
            String path;
            RUSH_DEFINE_PROPERTIES_NON_INTRUSIVE_WITH_DEFAULT(Binding, binding, type, target, path);
        };
        DArray<Binding> uniforms;

        RUSH_DEFINE_PROPERTIES_NON_INTRUSIVE_WITH_DEFAULT(MaterialParse, type, shading_model, vs, fs, depth_write, depth_test, uniforms);
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
            //m_DefaulLitData->albedo = Vector4(1.0f);
        }
        else if (matData.shading_model == "unlit")
        {
            m_ShadingModel = ShadingModel::Unlit;
        }

        depthTest = matData.depth_test;
        depthWrite = matData.depth_write;

        std::string defines;

        RUSH_ASSERT(matData.vs != "" && matData.fs != "");
        AssetsManager::instance().LoadOrCompileShader(matData.vs, defines, [&](AssetLoadResult result, Ref<RShader> shader, void* param) {
            m_VertexShader = shader;
        }, nullptr);

        AssetsManager::instance().LoadOrCompileShader(matData.fs, defines, [&](AssetLoadResult result, Ref<RShader> shader, void* param) {
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
                bindingGroupLayouts.push_back(renderer->GetFrameDataGroup().GetBindLayoutHandle());
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
                        static auto samp = CreateRef<RSampler>();
                        material->m_BindGroup->AddBinding(info.binding, ShaderStage::Vertex | ShaderStage::Fragment, samp, wgpu::SamplerBindingType::Filtering);
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

                    if (material->GetBlendMode() == BlendMode::AlphaBlend)
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
                primitive->frontFace = FrontFace::CCW;
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

//             static char const triangle_vert_wgsl[] = R"(
// 	        struct VertexIn {
// 		        @location(0) aPos : vec3<f32>,
// 		        @location(1) aUV  : vec2<f32>,
// 	        }
// 	        struct VertexOut {
// 		        @location(0) vUV  : vec2<f32>,
// 		        @builtin(position) Position : vec4<f32>
// 	        }
//             struct GlobalUniforms {
//                 @location(0) uView0 : vec4<f32>,
//                 @location(1) uView1 : vec4<f32>,
//                 @location(2) uView2 : vec4<f32>,
//                 @location(3) uView3 : vec4<f32>,
//                 @location(4) uProj0 : vec4<f32>,
//                 @location(5) uProj1 : vec4<f32>,
//                 @location(6) uProj2 : vec4<f32>,
//                 @location(7) uProj3 : vec4<f32>
//             }
// 	        @group(0) @binding(0) var<uniform> uGlobalUniforms : GlobalUniforms;
// 	        @vertex
// 	        fn main(input : VertexIn) -> VertexOut {
// 		        var output : VertexOut;
//                 var view = mat4x4<f32>(uGlobalUniforms.uView0, uGlobalUniforms.uView1, uGlobalUniforms.uView2, uGlobalUniforms.uView3);
//                 var proj = mat4x4<f32>(uGlobalUniforms.uProj0, uGlobalUniforms.uProj1, uGlobalUniforms.uProj2, uGlobalUniforms.uProj3);
// 		        output.Position = proj * view * vec4<f32>(input.aPos, 1.0);
// 		        output.vUV = input.aUV;
// 		        return output;
// 	        }
//         )";
// 
//             static char const triangle_frag_wgsl[] = R"(
//         	@group(0) @binding(1) var mySampler : sampler;
// 			@group(0) @binding(2) var myTexture : texture_2d<f32>;
// 	        @fragment
// 	        fn main(@location(0) vUV : vec2<f32>) -> @location(0) vec4<f32> {
// 		        var color = textureSample(myTexture, mySampler, vUV);
//                 color = pow(abs(color), vec4<f32>(2.2)); // to linear 
//                 color = pow(abs(color), vec4<f32>(1.0/2.2)); // to gamma
//                 return color;
// 	        }
//         )";
// 
//             Ref<RShader> vs = CreateRef<RShader>(ShaderStage::Vertex, triangle_vert_wgsl, "vs_test");
//             Ref<RShader> fs = CreateRef<RShader>(ShaderStage::Fragment, triangle_frag_wgsl, "fs_test");
// 
//             PipelineDesc pipeDesc = {};
//             pipeDesc.depthWrite = true;
//             pipeDesc.depthTest = true;
//             pipeDesc.colorFormat = TextureFormat::BGRA8Unorm;
//             pipeDesc.depthStencilFormat = TextureFormat::Depth24PlusStencil8;
//             pipeDesc.depthCompare = CompareFunction::Less;
// 
//             VertexAttribute_t vertAttrs[2];
//             vertAttrs[0].format = VertexFormat::Float32x3;
//             vertAttrs[0].offset = 0;
//             vertAttrs[0].shaderLocation = 0;
//             vertAttrs[1].format = VertexFormat::Float32x2;
//             vertAttrs[1].offset = 0;
//             vertAttrs[1].shaderLocation = 1;
// 
//             auto& vLayout0 = pipeDesc.vertexLayouts.emplace_back();
//             vLayout0.stride = sizeof(float) * 3;
//             vLayout0.attributes = &vertAttrs[0];
//             vLayout0.attributeCount = 1;
// 
//             auto& vLayout1 = pipeDesc.vertexLayouts.emplace_back();
//             vLayout1.stride = sizeof(float) * 2;
//             vLayout1.attributes = &vertAttrs[1];
//             vLayout1.attributeCount = 1;
// 
//             pipeDesc.vs = vs;
//             pipeDesc.fs = fs;
//             pipeDesc.writeMask = ColorWriteMask::All;
// 
//             auto l = {
//                 BindingLayoutHelper_t(0, ShaderStage::Vertex, BufferBindingType::Uniform),
//                 BindingLayoutHelper_t(1, ShaderStage::Fragment, SamplerBindingType::Filtering),
//                 BindingLayoutHelper_t(2, ShaderStage::Fragment, TextureSampleType::Float, TextureViewDimension::e2D)
//             };
//             Ref<BindingLayout> bindingLayout = CreateRef<BindingLayout>(l);
// 
//             Ref<RTexture> tex;
//             AssetsManager::instance().LoadTexture("assets/Bricks051_2K_Color.jpg", [&](AssetLoadResult result, Ref<RTexture> texture, void* param) {
//                 tex = texture;
//             });
// 
//             pipeDesc.bindLayout = bindingLayout;
//             pipeDesc.primitiveType = PrimitiveTopology::TriangleList;
//             pipeDesc.frontFace = FrontFace::CW;
//             pipeDesc.cullModel = CullMode::Back;


        }
    }


#if 0
    Ref<RUniformBuffer> RMaterialInst::s_GlobalUniformBuffer;

    bool RMaterialInst::Load(const StringView& path)
    {

        static char const triangle_vert_wgsl[] = R"(
	        struct VertexIn {
		        @location(0) aPos : vec3<f32>,
		        @location(1) aUV  : vec2<f32>,
	        }
	        struct VertexOut {
		        @location(0) vUV  : vec2<f32>,
		        @builtin(position) Position : vec4<f32>
	        }
            struct GlobalUniforms {
                @location(0) uView0 : vec4<f32>,
                @location(1) uView1 : vec4<f32>,
                @location(2) uView2 : vec4<f32>,
                @location(3) uView3 : vec4<f32>,
                @location(4) uProj0 : vec4<f32>,
                @location(5) uProj1 : vec4<f32>,
                @location(6) uProj2 : vec4<f32>,
                @location(7) uProj3 : vec4<f32>
            }
	        @group(0) @binding(0) var<uniform> uGlobalUniforms : GlobalUniforms;
	        @vertex
	        fn main(input : VertexIn) -> VertexOut {
		        var output : VertexOut;
                var view = mat4x4<f32>(uGlobalUniforms.uView0, uGlobalUniforms.uView1, uGlobalUniforms.uView2, uGlobalUniforms.uView3);
                var proj = mat4x4<f32>(uGlobalUniforms.uProj0, uGlobalUniforms.uProj1, uGlobalUniforms.uProj2, uGlobalUniforms.uProj3);
		        output.Position = proj * view * vec4<f32>(input.aPos, 1.0);
		        output.vUV = input.aUV;
		        return output;
	        }
        )";

        static char const triangle_frag_wgsl[] = R"(
        	@group(0) @binding(1) var mySampler : sampler;
			@group(0) @binding(2) var myTexture : texture_2d<f32>;
	        @fragment
	        fn main(@location(0) vUV : vec2<f32>) -> @location(0) vec4<f32> {
		        var color = textureSample(myTexture, mySampler, vUV);
                color = pow(abs(color), vec4<f32>(2.2)); // to linear 
                color = pow(abs(color), vec4<f32>(1.0/2.2)); // to gamma
                return color;
	        }
        )";

        Ref<RShader> vs = CreateRef<RShader>(ShaderStage::Vertex, triangle_vert_wgsl, "vs_test");
        Ref<RShader> fs = CreateRef<RShader>(ShaderStage::Fragment, triangle_frag_wgsl, "fs_test");

        PipelineDesc pipeDesc = {};
        pipeDesc.depthWrite = true;
        pipeDesc.depthTest = true;
        pipeDesc.colorFormat = TextureFormat::BGRA8Unorm;
        pipeDesc.depthStencilFormat = TextureFormat::Depth24PlusStencil8;
        pipeDesc.depthCompare = CompareFunction::Less;

        VertexAttribute vertAttrs[2];
        vertAttrs[0].format = VertexFormat::Float32x3;
        vertAttrs[0].offset = 0;
        vertAttrs[0].shaderLocation = 0;
        vertAttrs[1].format = VertexFormat::Float32x2;
        vertAttrs[1].offset = 0;
        vertAttrs[1].shaderLocation = 1;

        auto& vLayout0 = pipeDesc.vertexLayouts.emplace_back();
        vLayout0.stride = sizeof(float) * 3;
        vLayout0.attributes = &vertAttrs[0];
        vLayout0.attributeCount = 1;

        auto& vLayout1 = pipeDesc.vertexLayouts.emplace_back();
        vLayout1.stride = sizeof(float) * 2;
        vLayout1.attributes = &vertAttrs[1];
        vLayout1.attributeCount = 1;

        pipeDesc.vs = vs;
        pipeDesc.fs = fs;
        pipeDesc.writeMask = ColorWriteMask::All;

        auto l = { 
            BindingLayoutHelper(0, ShaderStage::Vertex, BufferBindingType::Uniform),
            BindingLayoutHelper(1, ShaderStage::Fragment, SamplerBindingType::Filtering),
            BindingLayoutHelper(2, ShaderStage::Fragment, TextureSampleType::Float, TextureViewDimension::e2D)
        };
        Ref<BindingLayout> bindingLayout = CreateRef<BindingLayout>(l);

        Ref<RTexture> tex;
        AssetsManager::instance().LoadTexture("assets/Bricks051_2K_Color.jpg", [&](AssetLoadResult result, Ref<RTexture> texture, void* param) {
            tex = texture;
        });

        pipeDesc.bindLayout = bindingLayout;
        pipeDesc.primitiveType = PrimitiveTopology::TriangleList;
        pipeDesc.frontFace = FrontFace::CW;
        pipeDesc.cullModel = CullMode::Back;

//         float rotDeg = 0.0f;
//         auto uniformBuf = CreateRef<RUniformBuffer>(BufferUsage::Uniform, sizeof(rotDeg), "uniforms0");
//         uniformBuf->UpdateData(&rotDeg, sizeof(rotDeg));
        if (s_GlobalUniformBuffer == nullptr)
        {   
            s_GlobalUniformBuffer = CreateRef<RUniformBuffer>(sizeof(Matrix4) * 2, "GlobalUniforms");
        }

        auto layout = { 
            BindingInitializationHelper(0, s_GlobalUniformBuffer),
            BindingInitializationHelper(1, CreateRef<RSampler>()),
            BindingInitializationHelper(2, tex)
        };
        auto bindGroup = CreateRef<RBindGroup>(bindingLayout, layout, "bindgroup0");

        m_Material = CreateRef<RMaterial>();
        m_Material->m_Pipeline = CreateRef<RPipeline>(pipeDesc, "pipeline_1");
        m_UniformBuffer = nullptr;
        m_BindGroup = bindGroup;

        return true;
    }
    #endif
}