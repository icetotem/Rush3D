#include "stdafx.h"
#include <string.h>
#include "Engine.h"
#include "Window.h"
#include "render/Renderer.h"
#include "render/RTexture.h"
#include "render/RBuffer.h"
#include "render/Shader.h"
#include "render/Uniform.h"
#include "render/RPipeline.h"
#include "render/RenderBatch.h"

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
Ref<RenderContent> content = CreateRef<RenderContent>();
Ref<RenderPass> pass0;
Ref<RenderPass> pass1;
Ref<BindGroup> bindGroup;
Ref<UniformBuffer> uniformBuf;

static bool redraw2()
{
	if (g_Renderer)
	{
		g_Renderer->BeginDraw();

		rotDeg += 0.1f;

		g_Renderer->DrawOfflinePass(pass0, content);
		g_Renderer->DrawFinalPass(content);

		g_Renderer->EndDraw();
	}
    return true;
}


int main(int argc, char* argv[]) 
{	
	Engine engine;
	
	WindowDesc wndDesc = 
	{
		.Title = "RushDmeo",
		.Width = 1280,
		.Height = 900,
        .OnTop = false,
        .Visible = false,
	};

	auto window = engine.CreateRenderWindow(wndDesc);
	
	if (window)
    {
		window->Show(true);

		RendererDesc rendererDesc;
		rendererDesc.msaa = 1;
		auto renderer = engine.CreateRenderer(window, &rendererDesc);
		g_Renderer = renderer;

		RenderPassDesc rpDesc = {
			.width = 1280,
			.height = 900,
		};

		pass0 = renderer->CreateRenderPass(&rpDesc);


        Ref<Shader> vs = renderer->CreateShader(triangle_vert_wgsl, ShaderStage::Vertex);
        Ref<Shader> fs = renderer->CreateShader(triangle_frag_wgsl, ShaderStage::Fragment);


        PipelineDesc pipeDesc;

		pipeDesc.ColorFormat = TextureFormat::RGBA8Unorm;
		pipeDesc.DepthFormat = TextureFormat::Depth24PlusStencil8;

        VertexAttribute vertAttrs[2];
        vertAttrs[0].Format = VertexFormat::Float32x2;
        vertAttrs[0].Offset = 0;
        vertAttrs[0].ShaderLocation = 0;
        vertAttrs[1].Format = VertexFormat::Float32x3;
        vertAttrs[1].Offset = 0;
        vertAttrs[1].ShaderLocation = 1;

        auto& vLayout0 = pipeDesc.VLayouts.emplace_back();
        vLayout0.Stride = sizeof(float) * 2;
        vLayout0.Attributes = &vertAttrs[0];
        vLayout0.AttributeCount = 1;

        auto& vLayout1 = pipeDesc.VLayouts.emplace_back();
        vLayout1.Stride = sizeof(float) * 3;
        vLayout1.Attributes = &vertAttrs[1];
        vLayout1.AttributeCount = 1;

        pipeDesc.VS = vs;
        pipeDesc.FS = fs;
        pipeDesc.WriteMask = ColorWriteMask::Write_All;

		Ref<BindingLayout> bindingLayout = renderer->CreateBindingLayout({
			{0, ShaderStage::Vertex, BufferBindingType::Uniform}
		});

		pipeDesc.BindLayout = bindingLayout;

        Ref<RenderPipeline> rpipe = renderer->CreatePipeline(&pipeDesc);

        // create the buffers (x, y)
        float const vertData0[] = {
            -0.8f, -0.8f, // BL
             0.8f, -0.8f, // BR
            -0.0f,  0.8f, // top
        };

        // create the buffers (r, g, b)
        float const vertData1[] = {
            1.0f, 1.0f, 1.0f, // BL
            1.0f, 1.0f, 1.0f, // BR
            1.0f, 1.0f, 1.0f, // top
        };

        uint16_t const indxData[] = {
            0, 1, 2
        };


        Ref<RVertexBuffer> vb0 = renderer->CreateVertexBuffer(sizeof(float) * 2, sizeof(vertData0));
		vb0->UpdateData(vertData0, sizeof(vertData0));

        Ref<RVertexBuffer> vb1 = renderer->CreateVertexBuffer(sizeof(float) * 3, sizeof(vertData1));
		vb1->UpdateData(vertData1, sizeof(vertData1));

        Ref<RIndexBuffer> ib = renderer->CreateIndexBuffer(3, false);

		uniformBuf = renderer->CreateUniformBuffer(sizeof(rotDeg));
		uniformBuf->UpdateData(&rotDeg, sizeof(rotDeg));

		bindGroup = renderer->CreateBindGroup(bindingLayout, {
			{0, uniformBuf}
		});

		Ref<RenderBatch> batch = CreateRef<RenderBatch>();
        batch->Pipeline = rpipe;
        batch->VBList.push_back(vb0);
        batch->VBList.push_back(vb1);
        batch->IB = ib;
		batch->Uniforms = bindGroup;
		content->m_Batches.push_back(batch);

        while (window->ShouldClose())
        {
			window->MessgeLoop();
			redraw2();
        }
	}

	g_Renderer.reset();
	window.reset();

	return 0;
}
