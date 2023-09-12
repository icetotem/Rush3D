#include "stdafx.h"
#include "render/RMaterial.h"
#include "AssetManager.h"

namespace rush
{

    Ref<RUniformBuffer> RMaterialInst::s_GlobalUniformBuffer;

    bool RMaterial::Load(const StringView& path)
    {

        return true;
    }


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
}