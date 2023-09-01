#include "stdafx.h"
#include "Engine.h"
#include "Window.h"
#include "render/Renderer.h"
#include "render/RTexture.h"
#include "render/RBuffer.h"
#include "render/RShader.h"
#include "render/RUniform.h"
#include "render/RPipeline.h"
#include "render/RBatch.h"
#include "components/EcsSystem.h"
#include "components/Transform.h"

using namespace rush;

/**
 * Current rotation angle (in degrees, updated per frame).
 */
float rotDeg = 0.0f;

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

Ref<Renderer> g_Renderer = nullptr;
Ref<RContent> content0 = CreateRef<RContent>();
Ref<RPass> pass0;
Ref<RPass> pass1;
Ref<RBindGroup> bindGroup;
Ref<RUniformBuffer> uniformBuf;
Ref<RScreenQuad> sQuad1;
Ref<RScreenQuad> sQuadFinal;

static bool render()
{
	if (g_Renderer)
	{
		g_Renderer->BeginDraw(Vector4(0.5, 0.5, 1, 1));

		rotDeg += 0.1f;

		uniformBuf->UpdateData(&rotDeg, sizeof(rotDeg));

		g_Renderer->DrawOffScreenPass(pass0, content0);
		g_Renderer->DrawOffScreenQuad(pass1, sQuad1);
		g_Renderer->DrawFinalScreenQuad(sQuadFinal);
		//        g_Renderer->DrawFinalPass(content0);

		g_Renderer->EndDraw();
	}
	return true;
}


int main(int argc, char* argv[])
{
	Engine engine;
	engine.Init();

	WindowDesc wndDesc;
	wndDesc.title = "RushDmeo";
	wndDesc.width = 1280;
	wndDesc.height = 900;
	wndDesc.alwaysTop = false;
	wndDesc.visible = false;

    RendererDesc rendererDesc;
    rendererDesc.clearColor = Vector4(0.0f, 0.0f, 0.2f, 1.0f);

	auto window = engine.CreateRenderWindow(wndDesc, rendererDesc);
    g_Renderer = window->GetRenderer();

	if (window)
	{
		window->Show(true);

		RenderPassDesc rpDesc;
		rpDesc.width = 1280;
		rpDesc.height = 900;
		rpDesc.useDepthStencil = true;
		rpDesc.clearDepth = 1.0f;
		rpDesc.colorFormat = TextureFormat::BGRA8Unorm;
		rpDesc.depthStencilFormat = TextureFormat::Depth24PlusStencil8;

		rpDesc.useDepthStencil = true;
		rpDesc.clearColor = Vector4(0.0f, 0.2f, 0.0f, 1.0f);
		pass0 = CreateRef<RPass>(rpDesc);

		rpDesc.useDepthStencil = false;
		rpDesc.clearColor = Vector4(0.2f, 0.0f, 0.0f, 1.0f);
		pass1 = CreateRef<RPass>(rpDesc);

		// create screen quad
		{
			// create screen quad
			const char fs_code[] = R"(
				@group(0) @binding(0) var mySampler : sampler;
				@group(0) @binding(1) var myTexture : texture_2d<f32>;
				@fragment 
				fn main(@location(0) vUV : vec2<f32>) -> @location(0) vec4f {
					//return vec4<f32>(0.99, 0.99, 0.99, 1.0);
					return textureSample(myTexture, mySampler, vUV) * 1.2;
				}
			)";
			auto sColorFS = CreateRef<RShader>(ShaderStage::Fragment, fs_code, "screen_quad_fs");

            auto l = {
                BindingLayoutHelper(0, ShaderStage::Fragment, SamplerBindingType::Filtering),
                BindingLayoutHelper(1, ShaderStage::Fragment, TextureSampleType::Float, TextureViewDimension::e2D)
            };

			Ref<BindingLayout> bl = CreateRef<BindingLayout>(l);

            auto ll = {
                BindingInitializationHelper(0, CreateRef<RSampler>()),
                BindingInitializationHelper(1, pass0->GetColorTexture())
            };

			auto quadBindGroup = CreateRef<RBindGroup>(bl, ll, "screen_quad_bindgroup");

			sQuad1 = g_Renderer->CreateScreenQuad(sColorFS, quadBindGroup);
		}

		{
			const char fs_code[] = R"(
				@group(0) @binding(0) var mySampler : sampler;
				@group(0) @binding(1) var myTexture : texture_2d<f32>;
				@fragment 
				fn main(@location(0) vUV : vec2<f32>) -> @location(0) vec4f {
					//return vec4<f32>(0.5, 0.5, 0.5, 1.0);
					return textureSample(myTexture, mySampler, vUV) * 1.2;
				}
			)";
			auto sColorFS = CreateRef<RShader>(ShaderStage::Fragment, fs_code, "screen_quad_fs");

            auto l = {
                BindingLayoutHelper(0, ShaderStage::Fragment, SamplerBindingType::Filtering),
                BindingLayoutHelper(1, ShaderStage::Fragment, TextureSampleType::Float, TextureViewDimension::e2D)
            };
			Ref<BindingLayout> bl = CreateRef<BindingLayout>(l);

            auto ll = {
                BindingInitializationHelper(0, CreateRef<RSampler>()),
                BindingInitializationHelper(1, pass1->GetColorTexture())
			};

			auto quadBindGroup = CreateRef<RBindGroup>(bl, ll, "screen_quad_bindgroup");

			sQuadFinal = g_Renderer->CreateScreenQuad(sColorFS, quadBindGroup);
		}



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
		vertAttrs[1].format = VertexFormat::Float32x3;
		vertAttrs[1].offset = 0;
		vertAttrs[1].shaderLocation = 1;

		auto& vLayout0 = pipeDesc.vertexLayouts.emplace_back();
		vLayout0.stride = sizeof(float) * 2;
		vLayout0.attributes = &vertAttrs[0];
		vLayout0.attributeCount = 1;

		auto& vLayout1 = pipeDesc.vertexLayouts.emplace_back();
		vLayout1.stride = sizeof(float) * 3;
		vLayout1.attributes = &vertAttrs[1];
		vLayout1.attributeCount = 1;

		pipeDesc.vs = vs;
		pipeDesc.fs = fs;
		pipeDesc.writeMask = ColorWriteMask::Write_All;

		auto l = { BindingLayoutHelper(0, ShaderStage::Vertex, BufferBindingType::Uniform) };
		Ref<BindingLayout> bindingLayout = CreateRef<BindingLayout>(l);

		pipeDesc.bindLayout = bindingLayout;
		pipeDesc.primitiveType = PrimitiveType::TriangleList;
		pipeDesc.frontFace = FrontFace::CCW;
		pipeDesc.cullModel = CullMode::Back;

		Ref<RPipeline> rpipe1 = CreateRef<RPipeline>(pipeDesc, "pipeline_1");

		// create the buffers (x, y)
		float const vertData0[] = {
			-0.8f, -0.8f, // BL
			 0.8f, -0.8f, // BR
			-0.0f,  0.8f, // top
		};

		// create the buffers (r, g, b)
		float const vertData1[] = {
			0.7f, 0.7f, 0.7f, // BL
			0.7f, 0.7f, 0.7f, // BR
			0.7f, 0.7f, 0.7f, // top
		};

		uint16_t const indxData[] = {
			0, 1, 2, 4
		};


		Ref<RVertexBuffer> vb0 = CreateRef<RVertexBuffer>(sizeof(float) * 2, sizeof(vertData0), "vb0");
		vb0->UpdateData(vertData0, sizeof(vertData0));

		Ref<RVertexBuffer> vb1 = CreateRef<RVertexBuffer>(sizeof(float) * 3, sizeof(vertData1), "vb1");
		vb1->UpdateData(vertData1, sizeof(vertData1));

		Ref<RIndexBuffer> ib = CreateRef<RIndexBuffer>(4, false, "ib0");
		ib->UpdateData(indxData, sizeof(indxData));

		uniformBuf = CreateRef<RUniformBuffer>(BufferUsage::Uniform, sizeof(rotDeg), "uniforms0");
		uniformBuf->UpdateData(&rotDeg, sizeof(rotDeg));

        auto layout = { BindingInitializationHelper(0, uniformBuf) };
		bindGroup = CreateRef<RBindGroup>(bindingLayout, layout, "bindgroup0");

		Ref<RBatch> batch0 = content0->NewBatch();
		batch0->pipeline = rpipe1;
        batch0->bindGroup = bindGroup;
        batch0->vertexBuffers.push_back(vb0);
		batch0->vertexBuffers.push_back(vb1);
		batch0->indexBuffer = ib;

		while (window->ShouldClose())
		{
			window->MessgeLoop();

			engine.Update();
			render();

			window->Present();
		}
	}

	pass0.reset();
	pass1.reset();
	sQuad1.reset();
	sQuadFinal.reset();
	content0.reset();
	bindGroup.reset();
	uniformBuf.reset();
	g_Renderer.reset();
	window.reset();

	engine.Shutdown();

	return 0;
}