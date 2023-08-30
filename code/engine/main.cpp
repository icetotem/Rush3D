#include "stdafx.h"
#include <string.h>
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
		g_Renderer->BeginDraw();

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
	
    WindowDesc wndDesc;
    wndDesc.title = "RushDmeo";
    wndDesc.width = 1280;
    wndDesc.height = 900;
    wndDesc.alwaysTop = false;
    wndDesc.visible = false;
	auto window = engine.CreateRenderWindow(wndDesc);
	
	if (window)
    {
		window->Show(true);

		RendererDesc rendererDesc;
		rendererDesc.backend = RenderBackend::D3D12;
		rendererDesc.clearColor = Vector4(0.0f, 0.0f, 0.2f, 1.0f);

		auto renderer = engine.CreateRenderer(window, &rendererDesc);
		g_Renderer = renderer;

		RenderPassDesc rpDesc;
		rpDesc.width = 1280;
		rpDesc.height = 900;		
		rpDesc.useDepthStencil = true;
		rpDesc.clearDepth = 1.0f;
        rpDesc.colorFormat = TextureFormat::BGRA8Unorm;
        rpDesc.depthStencilFormat = TextureFormat::Depth24PlusStencil8;

        rpDesc.useDepthStencil = true;
        rpDesc.clearColor = Vector4(0.0f, 0.2f, 0.0f, 1.0f);
        pass0 = renderer->CreateRenderPass(rpDesc);

        rpDesc.useDepthStencil = false;
        rpDesc.clearColor = Vector4(0.2f, 0.0f, 0.0f, 1.0f);
        pass1 = renderer->CreateRenderPass(rpDesc);

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
            auto sColorFS = renderer->CreateShader(fs_code, ShaderStage::Fragment, "screen_quad_fs");

            Ref<BindingLayout> bl = g_Renderer->CreateBindingLayout({
                { 0, ShaderStage::Fragment, SamplerBindingType::Filtering },
                { 1, ShaderStage::Fragment, TextureSampleType::Float, TextureViewDimension::e2D}
            });

            auto sampler = renderer->CreateSampler();

            auto quadBindGroup = g_Renderer->CreateBindGroup(bl, {
                { 0, sampler },
                { 1, pass0->GetColorTexture() }
                }, "screen_quad_bindgroup");

            sQuad1 = renderer->CreateScreenQuad(sColorFS, quadBindGroup);
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
			auto sColorFS = renderer->CreateShader(fs_code, ShaderStage::Fragment, "screen_quad_fs");
			auto sampler = renderer->CreateSampler();

            Ref<BindingLayout> bl = g_Renderer->CreateBindingLayout({
                { 0, ShaderStage::Fragment, SamplerBindingType::Filtering },
                { 1, ShaderStage::Fragment, TextureSampleType::Float, TextureViewDimension::e2D}
            });

			auto quadBindGroup = g_Renderer->CreateBindGroup(bl, {
				{ 0, sampler },
				{ 1, pass1->GetColorTexture() }
			}, "screen_quad_bindgroup");

			sQuadFinal = renderer->CreateScreenQuad(sColorFS, quadBindGroup);
		}



        Ref<RShader> vs = renderer->CreateShader(triangle_vert_wgsl, ShaderStage::Vertex, "vs_test");
        Ref<RShader> fs = renderer->CreateShader(triangle_frag_wgsl, ShaderStage::Fragment, "fs_test");

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

		Ref<BindingLayout> bindingLayout = renderer->CreateBindingLayout({
			{0, ShaderStage::Vertex, BufferBindingType::Uniform}
		});

		pipeDesc.bindLayout = bindingLayout;
		pipeDesc.primitiveType = PrimitiveType::TriangleList;
		pipeDesc.frontFace = FrontFace::CCW;
        pipeDesc.cullModel = CullMode::Back;

        Ref<RPipeline> rpipe1 = renderer->CreatePipeline(pipeDesc, "pipeline_1");

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


        Ref<RVertexBuffer> vb0 = renderer->CreateVertexBuffer(sizeof(float) * 2, sizeof(vertData0), "vb0");
		vb0->UpdateData(vertData0, sizeof(vertData0));

        Ref<RVertexBuffer> vb1 = renderer->CreateVertexBuffer(sizeof(float) * 3, sizeof(vertData1), "vb1");
		vb1->UpdateData(vertData1, sizeof(vertData1));

        Ref<RIndexBuffer> ib = renderer->CreateIndexBuffer(4, false, "ib0");
		ib->UpdateData(indxData, sizeof(indxData));

		uniformBuf = renderer->CreateUniformBuffer(sizeof(rotDeg), BufferUsage::Uniform, "uniforms0");
		uniformBuf->UpdateData(&rotDeg, sizeof(rotDeg));

		bindGroup = renderer->CreateBindGroup(bindingLayout, {
			{0, uniformBuf}
		}, "bindgroup0");

		Ref<RBatch> batch0 = content0->NewBatch();
        batch0->pipeline = rpipe1;
        batch0->vertexBufferList.push_back(vb0);
        batch0->vertexBufferList.push_back(vb1);
        batch0->indexBuffer = ib;
		batch0->uniforms = bindGroup;

		auto scene = engine.CreateScene("Scene1");

		// test ecs

		// create the entity
		auto ent0 = Entity::Create();
		auto trans = ent0.Add<Transform>();
		trans->SetPosition(0, 0, 0);

		ent0.Remove<Transform>();

        while (window->ShouldClose())
        {
            window->MessgeLoop();

            engine.Update();
            render();
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

	return 0;
}
