#include "stdafx.h"
#include "components/MaterialInst.h"

namespace rush
{

	/**
	 * WGSL equivalent of \c triangle_vert_spirv.
	 */
	static char const triangle_vert_wgsl[] = R"(
	struct VertexIn {
		@location(0) aPos : vec2<f32>,
		@location(1) aCol : vec3<f32>
	}
	struct VertexOut {
		@location(0) vCol : vec3<f32>,
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
		output.vCol = input.aCol;
		return output;
	}
	)";

	/**
	 * WGSL equivalent of \c triangle_frag_spirv.
	 */
	static char const triangle_frag_wgsl[] = R"(
	@fragment
	fn main(@location(0) vCol : vec3<f32>) -> @location(0) vec4<f32> {
		return vec4<f32>(vCol, 1.0);
	}
	)";


    MaterialInst::MaterialInst(Entity owner)
    {
        m_Vs = triangle_vert_wgsl;
        m_Fs = triangle_frag_wgsl;

		m_PiplineDesc = {};
        m_PiplineDesc.depthWrite = true;
        m_PiplineDesc.depthTest = false;
        m_PiplineDesc.colorFormat = TextureFormat::BGRA8Unorm;
        m_PiplineDesc.depthStencilFormat = TextureFormat::Depth24PlusStencil8;
        m_PiplineDesc.depthCompare = DepthCompareFunction::LessEqual;

        VertexAttribute vertAttrs[2];
        vertAttrs[0].format = VertexFormat::Float32x2;
        vertAttrs[0].offset = 0;
        vertAttrs[0].shaderLocation = 0;
        vertAttrs[1].format = VertexFormat::Float32x3;
        vertAttrs[1].offset = 0;
        vertAttrs[1].shaderLocation = 1;

        auto& vLayout0 = m_PiplineDesc.vertexLayouts.emplace_back();
        vLayout0.stride = sizeof(float) * 2;
        vLayout0.attributes = &vertAttrs[0];
        vLayout0.attributeCount = 1;

        auto& vLayout1 = m_PiplineDesc.vertexLayouts.emplace_back();
        vLayout1.stride = sizeof(float) * 3;
        vLayout1.attributes = &vertAttrs[1];
        vLayout1.attributeCount = 1;

//         Ref<RShader> vs = renderer->CreateShader(triangle_vert_wgsl, ShaderStage::Vertex, "vs_test");
//         Ref<RShader> fs = renderer->CreateShader(triangle_frag_wgsl, ShaderStage::Fragment, "fs_test");
//         m_PiplineDesc.vs = vs;
//         m_PiplineDesc.fs = fs;
        m_PiplineDesc.writeMask = ColorWriteMask::Write_All;

//         Ref<BindingLayout> bindingLayout = renderer->CreateBindingLayout({
//             {0, ShaderStage::Vertex, BufferBindingType::Uniform}
//         });

        //m_PiplineDesc.bindLayout = bindingLayout;
        m_PiplineDesc.primitiveType = PrimitiveType::TriangleList;
        m_PiplineDesc.frontFace = FrontFace::CCW;
        m_PiplineDesc.cullModel = CullMode::Back;
    }

    MaterialInst::~MaterialInst()
    {

    }

    void MaterialInst::SetAssets(const StringView& assetPath)
    {
        
    }

}
