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
Ref<RContent> content1 = CreateRef<RContent>();
Ref<RContent> content2 = CreateRef<RContent>();
Ref<RPass> pass0;
Ref<RPass> pass1;
Ref<RBindGroup> bindGroup;
Ref<RUniformBuffer> uniformBuf;

static bool render()
{
	if (g_Renderer)
	{
		g_Renderer->BeginDraw();

		rotDeg += 0.1f;

		uniformBuf->UpdateData(&rotDeg, sizeof(rotDeg));

		g_Renderer->DrawOffScreenPass(pass0, content2);
		g_Renderer->DrawFinalPass(content1);

		g_Renderer->EndDraw();
	}
    return true;
}


int main(int argc, char* argv[]) 
{	
	Engine engine;
	
    WindowDesc wndDesc;
    wndDesc.Title = "RushDmeo";
    wndDesc.Width = 1280;
    wndDesc.Height = 900;
    wndDesc.OnTop = false;
    wndDesc.Visible = false;

	auto window = engine.CreateRenderWindow(wndDesc);
	
	if (window)
    {
		window->Show(true);

		RendererDesc rendererDesc;
		rendererDesc.Backend = RenderBackend::D3D12;
		rendererDesc.Msaa = 1;

		auto renderer = engine.CreateRenderer(window, &rendererDesc);
		g_Renderer = renderer;

		RenderPassDesc rpDesc;
		rpDesc.Width = 1280;
		rpDesc.Height = 900;		
		rpDesc.WithDepthStencil = true;
		rpDesc.ClearColor = Vector4(0.3f, 0.4f, 0.3f, 1.0f);
		rpDesc.ClearDepth = 1.0f;
        rpDesc.ColorFormat = TextureFormat::BGRA8Unorm;
        rpDesc.DepthStencilFormat = TextureFormat::Depth24PlusStencil8;
		pass0 = renderer->CreateRenderPass(&rpDesc);

        Ref<RShader> vs = renderer->CreateShader(triangle_vert_wgsl, ShaderStage::Vertex, "vs_test");
        Ref<RShader> fs = renderer->CreateShader(triangle_frag_wgsl, ShaderStage::Fragment, "fs_test");


        PipelineDesc pipeDesc1 = {};
        pipeDesc1.DepthWrite = false;
        pipeDesc1.DepthTest = false;
		pipeDesc1.ColorFormat = TextureFormat::BGRA8Unorm;
		pipeDesc1.DepthFormat = TextureFormat::Depth24PlusStencil8;
		pipeDesc1.DepthCompare = DepthCompareFunction::LessEqual;

        VertexAttribute vertAttrs[2];
        vertAttrs[0].Format = VertexFormat::Float32x2;
        vertAttrs[0].Offset = 0;
        vertAttrs[0].ShaderLocation = 0;
        vertAttrs[1].Format = VertexFormat::Float32x3;
        vertAttrs[1].Offset = 0;
        vertAttrs[1].ShaderLocation = 1;

        auto& vLayout0 = pipeDesc1.VLayouts.emplace_back();
        vLayout0.Stride = sizeof(float) * 2;
        vLayout0.Attributes = &vertAttrs[0];
        vLayout0.AttributeCount = 1;

        auto& vLayout1 = pipeDesc1.VLayouts.emplace_back();
        vLayout1.Stride = sizeof(float) * 3;
        vLayout1.Attributes = &vertAttrs[1];
        vLayout1.AttributeCount = 1;

        pipeDesc1.VS = vs;
        pipeDesc1.FS = fs;
        pipeDesc1.WriteMask = ColorWriteMask::Write_All;

		Ref<BindingLayout> bindingLayout = renderer->CreateBindingLayout({
			{0, ShaderStage::Vertex, BufferBindingType::Uniform}
		});

		pipeDesc1.BindLayout = bindingLayout;
		pipeDesc1.Cull = CullMode::Back;
		pipeDesc1.Primitive = PrimitiveType::TriangleList;
		pipeDesc1.Front = FrontFace::CCW;

        Ref<RPipeline> rpipe1 = renderer->CreatePipeline(&pipeDesc1, "pipeline_1");

        PipelineDesc pipeDesc2 = pipeDesc1;
		pipeDesc2.DepthTest = true;
        Ref<RPipeline> rpipe2 = renderer->CreatePipeline(&pipeDesc2, "pipeline_2");


        // create the buffers (x, y)
        float const vertData0[] = {
            -0.8f, -0.8f, // BL
             0.8f, -0.8f, // BR
            -0.0f,  0.8f, // top
        };

        // create the buffers (r, g, b)
        float const vertData1[] = {
            1.0f, 0.0f, 0.0f, // BL
            0.0f, 1.0f, 0.0f, // BR
            0.0f, 0.0f, 1.0f, // top
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

		Ref<RBatch> batch1 = content1->NewBatch();
        batch1->Pipeline = rpipe1;
        batch1->VBList.push_back(vb0);
        batch1->VBList.push_back(vb1);
        batch1->IB = ib;
		batch1->Uniforms = bindGroup;

        Ref<RBatch> batch2 = content2->NewBatch();
        batch2->Pipeline = rpipe2;
        batch2->VBList.push_back(vb0);
        batch2->VBList.push_back(vb1);
        batch2->IB = ib;
        batch2->Uniforms = bindGroup;

        while (window->ShouldClose())
        {
			window->MessgeLoop();
			render();
        }
	}

	content1.reset();
	content2.reset();
	bindGroup.reset();
	uniformBuf.reset();
	g_Renderer.reset();
	window.reset();

	return 0;
}
