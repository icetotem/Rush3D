#include "stdafx.h"
#include "render/RMaterial.h"
#include "AssetManager.h"

namespace rush
{

    bool RMaterial::Load(const StringView& path)
    {

        return true;
    }


    bool RMaterialInst::Load(const StringView& path)
    {

        static char const triangle_vert_wgsl[] = R"(
	        struct VertexIn {
		        @location(0) aPos : vec2<f32>,
		        @location(1) aUV  : vec2<f32>,
	        }
	        struct VertexOut {
		        @location(0) vUV  : vec2<f32>,
		        @builtin(position) Position : vec4<f32>
	        }
	        struct Rotation {
		        @location(0) degs : f32
	        }
	        @group(0) @binding(0) var<uniform> uRot : Rotation;
	        @vertex
	        fn main(input : VertexIn) -> VertexOut {
		        var rads : f32 = radians(uRot.degs);
		        var cosA : f32 = cos(rads);
		        var sinA : f32 = sin(rads);
		        var rot : mat3x3<f32> = mat3x3<f32>(
			        vec3<f32>( cosA, sinA, 0.0),
			        vec3<f32>(-sinA, cosA, 0.0),
			        vec3<f32>( 0.0,  0.0,  1.0));
		        var output : VertexOut;
		        output.Position = vec4<f32>(rot * vec3<f32>(input.aPos, 1.0), 1.0);
		        output.vUV = input.aUV;
		        return output;
	        }
        )";

        static char const triangle_frag_wgsl[] = R"(
        	@group(0) @binding(1) var mySampler : sampler;
			@group(0) @binding(2) var myTexture : texture_2d<f32>;
	        @fragment
	        fn main(@location(0) vUV : vec2<f32>) -> @location(0) vec4<f32> {
		        return textureSample(myTexture, mySampler, vUV);
	        }
        )";

        Ref<RShader> vs = CreateRef<RShader>(ShaderStage::Vertex, triangle_vert_wgsl, "vs_test");
        Ref<RShader> fs = CreateRef<RShader>(ShaderStage::Fragment, triangle_frag_wgsl, "fs_test");

        PipelineDesc pipeDesc = {};
        pipeDesc.depthWrite = true;
        pipeDesc.depthTest = false;
        pipeDesc.colorFormat = TextureFormat::BGRA8Unorm;
        pipeDesc.depthStencilFormat = TextureFormat::Depth24PlusStencil8;
        pipeDesc.depthCompare = DepthCompareFunction::LessEqual;

        VertexAttribute vertAttrs[2];
        vertAttrs[0].format = VertexFormat::Float32x2;
        vertAttrs[0].offset = 0;
        vertAttrs[0].shaderLocation = 0;
        vertAttrs[1].format = VertexFormat::Float32x2;
        vertAttrs[1].offset = 0;
        vertAttrs[1].shaderLocation = 1;

        auto& vLayout0 = pipeDesc.vertexLayouts.emplace_back();
        vLayout0.stride = sizeof(float) * 2;
        vLayout0.attributes = &vertAttrs[0];
        vLayout0.attributeCount = 1;

        auto& vLayout1 = pipeDesc.vertexLayouts.emplace_back();
        vLayout1.stride = sizeof(float) * 2;
        vLayout1.attributes = &vertAttrs[1];
        vLayout1.attributeCount = 1;

        pipeDesc.vs = vs;
        pipeDesc.fs = fs;
        pipeDesc.writeMask = ColorWriteMask::Write_All;

        auto l = { 
            BindingLayoutHelper(0, ShaderStage::Vertex, BufferBindingType::Uniform),
            BindingLayoutHelper(1, ShaderStage::Fragment, SamplerBindingType::Filtering),
            BindingLayoutHelper(2, ShaderStage::Fragment, TextureSampleType::Float, TextureViewDimension::e2D)
        };
        Ref<BindingLayout> bindingLayout = CreateRef<BindingLayout>(l);

        Ref<RTexture> tex;
        AssetsManager::instance().LoadTexture("assets/BasicClock3_S.jpg", [&](AssetLoadResult result, Ref<RTexture> texture, void* param) {
            tex = texture;
        });

        pipeDesc.bindLayout = bindingLayout;
        pipeDesc.primitiveType = PrimitiveType::TriangleList;
        pipeDesc.frontFace = FrontFace::CCW;
        pipeDesc.cullModel = CullMode::Back;

        float rotDeg = 0.0f;

        auto uniformBuf = CreateRef<RUniformBuffer>(BufferUsage::Uniform, sizeof(rotDeg), "uniforms0");
        uniformBuf->UpdateData(&rotDeg, sizeof(rotDeg));

        auto layout = { 
            BindingInitializationHelper(0, uniformBuf),
            BindingInitializationHelper(1, CreateRef<RSampler>()),
            BindingInitializationHelper(2, tex)
        };
        auto bindGroup = CreateRef<RBindGroup>(bindingLayout, layout, "bindgroup0");

        m_Material = CreateRef<RMaterial>();
        m_Material->m_Pipeline = CreateRef<RPipeline>(pipeDesc, "pipeline_1");
        m_UniformBuffer = uniformBuf;
        m_BindGroup = bindGroup;

        return true;
    }
}