#include "stdafx.h"

#include <dawn/dawn_proc.h>

#include "render/Renderer.h"
#include "render/RShader.h"
#include "render/RDevice.h"
#include "components/Camera.h"


namespace rush
{

    Renderer::Renderer(uint32_t width, uint32_t height)
    {
        m_Width = width;
        m_Height = height;
        CreateFullScreenQuad();
    }

    void Renderer::CreateFullScreenQuad()
    {
        const char screen_quad_vs[] = R"(
	            struct VertexIn {
		            @location(0) aPos : vec2<f32>
	            }
	            struct VertexOut {
		            @location(0) vUV  : vec2<f32>,
		            @builtin(position) Position : vec4<f32>
	            }
	            @vertex
	            fn main(input : VertexIn) -> VertexOut {
		            var output : VertexOut;
		            output.Position = vec4<f32>(input.aPos, 1.0, 1.0);
                    var uv = (input.aPos + 1.0) * 0.5;
		            output.vUV = vec2<f32>(uv.x, 1.0 - uv.y);
		            return output;
	            })";

        m_QuadVS = CreateRef<RShader>(ShaderStage::Vertex, screen_quad_vs, "screen_quad_vs");

        const char final_quad_fs[] = R"(
        	    @group(0) @binding(0) var mySampler : sampler;
			    @group(0) @binding(1) var myTexture : texture_2d<f32>;
	            @fragment
	            fn main(@location(0) vUV : vec2<f32>) -> @location(0) vec4<f32> {
		            var color = textureSample(myTexture, mySampler, vUV);
                    return color;
	            })";

        m_QuadFSFinal = CreateRef<RShader>(ShaderStage::Vertex, final_quad_fs, "final_quad_fs");

        float const verts[] =
        {
            -1.f,  1.f, // TL
            -1.f, -1.f, // BL
             1.f,  1.f, // TR
             1.f, -1.f, // BR
        };

        m_QuadVB = CreateRef<RVertexBuffer>(sizeof(float) * 2, sizeof(verts), verts, "screen_quad_vb");

        PipelineDesc pipeDesc = {};
        pipeDesc.depthWrite = false;
        pipeDesc.depthTest = false;
        pipeDesc.colorFormat = TextureFormat::BGRA8Unorm;

        VertexAttribute vertAttr;
        vertAttr.format = VertexFormat::Float32x3;
        vertAttr.offset = 0;
        vertAttr.shaderLocation = 0;

        auto& vLayout0 = pipeDesc.vertexLayouts.emplace_back();
        vLayout0.stride = sizeof(float) * 3;
        vLayout0.attributes = &vertAttr;
        vLayout0.attributeCount = 1;

        auto l = {
            BindingLayoutHelper(0, ShaderStage::Fragment, SamplerBindingType::Filtering),
            BindingLayoutHelper(1, ShaderStage::Fragment, TextureSampleType::Float, TextureViewDimension::e2D)
        };
        Ref<BindingLayout> bindingLayout = CreateRef<BindingLayout>(l);

        pipeDesc.vs = m_QuadVS;
        pipeDesc.fs = m_QuadFSFinal;
        pipeDesc.writeMask = ColorWriteMask::All;
        pipeDesc.bindLayout = bindingLayout;
        pipeDesc.primitiveType = PrimitiveTopology::TriangleList;
        pipeDesc.frontFace = FrontFace::CW;
        pipeDesc.cullModel = CullMode::Back;

        Ref<RTexture> tex;
        auto layout = {
            BindingInitializationHelper(1, CreateRef<RSampler>()),
            BindingInitializationHelper(2, tex)
        };

        m_FinalPassPipeline = CreateRef<RPipeline>(pipeDesc);
        m_FinalPassBindGroup = CreateRef<RBindGroup>(bindingLayout, layout, "FinalPass BindGroup");
    }


//     Ref<RScreenQuad> RenderContext::CreateScreenQuad(Ref<RShader> fs, Ref<RBindGroup> bindGroup)
//     {
//         Ref<RScreenQuad> sQuad = CreateRef<RScreenQuad>();
//         if (m_QuadVS == nullptr)
//         {
//             const char screen_quad_vs[] = R"(
// 	            struct VertexIn {
// 		            @location(0) aPos : vec2<f32>
// 	            }
// 	            struct VertexOut {
// 		            @location(0) vUV  : vec2<f32>,
// 		            @builtin(position) Position : vec4<f32>
// 	            }
// 	            @vertex
// 	            fn main(input : VertexIn) -> VertexOut {
// 		            var output : VertexOut;
// 		            output.Position = vec4<f32>(input.aPos, 1.0, 1.0);
//                     var uv = (input.aPos + 1.0) * 0.5;
// 		            output.vUV = vec2<f32>(uv.x, 1.0 - uv.y);
// 		            return output;
// 	            }
//             )";
//             m_QuadVS = CreateRef<RShader>(ShaderStage::Vertex, screen_quad_vs, "screen_quad_vs");
//         }
// 
//         if (m_QuadVB == nullptr) 
//         {
//             float const verts[] =
//             {
//                 -1.f,  1.f, // TL
//                 -1.f, -1.f, // BL
//                  1.f,  1.f, // TR
//                  1.f, -1.f, // BR
//             };
// 
//             m_QuadVB = CreateRef<RVertexBuffer>(sizeof(float) * 2, sizeof(verts), verts, "screen_quad_vb");
//         }
// 
//         PipelineDesc pipeDesc = {};
//         pipeDesc.depthWrite = false;
//         pipeDesc.depthTest = false;
//         pipeDesc.colorFormat = TextureFormat::BGRA8Unorm;
// 
//         VertexAttribute vertAttrs[2];
//         vertAttrs[0].format = VertexFormat::Float32x2;
//         vertAttrs[0].offset = 0;
//         vertAttrs[0].shaderLocation = 0;
// 
//         auto& vLayout = pipeDesc.vertexLayouts.emplace_back();
//         vLayout.stride = sizeof(float) * 2;
//         vLayout.attributes = &vertAttrs[0];
//         vLayout.attributeCount = 1;
// 
//         pipeDesc.vs = m_QuadVS;
//         pipeDesc.fs = fs;
//         pipeDesc.writeMask = ColorWriteMask::All;
// 
//         pipeDesc.bindLayout = bindGroup->GetBindLayout();
//         pipeDesc.primitiveType = PrimitiveTopology::TriangleStrip;
//         pipeDesc.frontFace = FrontFace::CCW;
//         pipeDesc.cullModel = CullMode::Back;
// 
//         sQuad->pipeline = CreateRef<RPipeline>(pipeDesc, "screen_quad_pipeline");
//         sQuad->bindGroup = bindGroup;
//         return sQuad;
//     }



//     void RenderContext::DrawOffScreenPass(Ref<RPass> renderPass, Ref<RenderQueue> content)
//     {
//         wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderPass->m_RenderPassDesc);
//         pass.SetScissorRect(m_Viewport.x * m_Width, m_Viewport.y * m_Height, (m_Viewport.z - m_Viewport.x) * m_Width, (m_Viewport.w - m_Viewport.y) * m_Height);
//         for (const auto batch : content->m_Batches)
//         {
//             pass.SetPipeline(batch->pipeline->GetPipeline());
//             pass.SetBindGroup(0, batch->bindGroup->GetBindGroup());
//             int vbIdx = 0;
//             for (auto vb : batch->vertexBuffers)
//             {
//                 pass.SetVertexBuffer(vbIdx, vb->m_Buffer);
//                 ++vbIdx;
//             }
//             pass.SetIndexBuffer(batch->indexBuffer->m_Buffer, batch->indexBuffer->GetType());
//             pass.DrawIndexed(batch->indexBuffer->GetIndexCount(), batch->instanceCount, 0, 0);
//         }
//         pass.End();
//     }
// 
//     void RenderContext::DrawOffScreenQuad(Ref<RPass> renderPass, Ref<RScreenQuad> sQuad)
//     {
//         wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderPass->m_RenderPassDesc);
//         pass.SetPipeline(sQuad->pipeline->GetPipeline());
//         pass.SetBindGroup(0, sQuad->bindGroup->GetBindGroup());
//         pass.SetVertexBuffer(0, m_QuadVB->m_Buffer);
//         pass.Draw(4);
//         pass.End();
//     }
// 
//     void RenderContext::DrawFinalScreenQuad(Ref<RScreenQuad> sQuad)
//     {
//         wgpu::TextureView backbufferView = swapChain.GetCurrentTextureView();
//         wgpu::RenderPassDescriptor renderPassDesc = {};
//         wgpu::RenderPassColorAttachment attachment = {};
//         attachment.view = backbufferView;
//         attachment.resolveTarget = nullptr;
//         attachment.loadOp = wgpu::LoadOp::Clear;
//         attachment.storeOp = wgpu::StoreOp::Store;
//         attachment.clearValue = { m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a };
//         renderPassDesc.colorAttachmentCount = 1;
//         renderPassDesc.colorAttachments = &attachment;
//         renderPassDesc.depthStencilAttachment = nullptr;
// 
//         wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderPassDesc);
//         pass.SetPipeline(sQuad->pipeline->GetPipeline());
//         pass.SetBindGroup(0, sQuad->bindGroup->GetBindGroup());
//         pass.SetVertexBuffer(0, m_QuadVB->m_Buffer);
//         pass.Draw(4);
//         pass.End();
//     }
// 
//     void RenderContext::DrawFinalPass(Ref<RenderQueue> content)
//     {
//         wgpu::TextureView backbufferView = swapChain.GetCurrentTextureView();
//         wgpu::RenderPassDescriptor renderPassDesc = {};
//         wgpu::RenderPassColorAttachment attachment = {};
//         attachment.view = backbufferView;
//         attachment.resolveTarget = nullptr;
//         attachment.loadOp = wgpu::LoadOp::Clear;
//         attachment.storeOp = wgpu::StoreOp::Store;
//         attachment.clearValue = { m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a };
//         renderPassDesc.colorAttachmentCount = 1;
//         renderPassDesc.colorAttachments = &attachment;
// 
//         wgpu::RenderPassDepthStencilAttachment depthStencilDesc;
//         depthStencilDesc.view = depthStencilView;
//         depthStencilDesc.depthReadOnly = false;
//         depthStencilDesc.stencilReadOnly = false;
//         depthStencilDesc.depthClearValue = 1.0f;
//         depthStencilDesc.stencilClearValue = 0;
//         depthStencilDesc.depthLoadOp = wgpu::LoadOp::Clear;
//         depthStencilDesc.depthStoreOp = wgpu::StoreOp::Store;
//         depthStencilDesc.stencilLoadOp = wgpu::LoadOp::Clear;
//         depthStencilDesc.stencilStoreOp = wgpu::StoreOp::Store;
//         renderPassDesc.depthStencilAttachment = &depthStencilDesc; 
// 
//         wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderPassDesc);
//         for (const auto batch : content->m_Batches)
//         {
//             pass.SetPipeline(batch->pipeline->GetPipeline());
//             pass.SetBindGroup(0, batch->bindGroup->GetBindGroup());
//             int vbIdx = 0;
//             for (auto vb : batch->vertexBuffers)
//             {
//                 pass.SetVertexBuffer(vbIdx, vb->m_Buffer);
//                 ++vbIdx;
//             }
//             pass.SetIndexBuffer(batch->indexBuffer->m_Buffer, batch->indexBuffer->GetType());
//             pass.DrawIndexed(batch->indexBuffer->GetIndexCount(), batch->instanceCount, 0, 0);
//         }
//         pass.End();
//     }

    void Renderer::BeginDraw()
    {
        dawn::native::InstanceProcessEvents(RDevice::instance().GetDawnInstance().Get());
        encoder = RDevice::instance().GetDevice().CreateCommandEncoder();
    }

    void Renderer::DrawScene(Ref<RenderQueue> renderQueue, const FrameBufferInfo& frameBuffer)
    {
        wgpu::RenderPassDescriptor renderPassDesc = {};
        renderPassDesc.label = frameBuffer.lable.c_str();

        wgpu::RenderPassColorAttachment attachments[8] = {};
        wgpu::RenderPassDepthStencilAttachment depthStencilDesc;

        bool useBackbuffer = false;
        if (frameBuffer.colorAttachment.size() > 0)
        {
            for (int i = 0; i < frameBuffer.colorAttachment.size(); ++i)
            {
                attachments[i].view = frameBuffer.colorAttachment[i].texture->GetTexture().CreateView();
                attachments[i].resolveTarget = nullptr;
                attachments[i].loadOp = wgpu::LoadOp::Clear;
                attachments[i].storeOp = wgpu::StoreOp::Store;

                const auto& color = frameBuffer.colorAttachment[i].clearColor;
                attachments[i].clearValue = { color.r, color.g, color.b, color.a };
            }
            renderPassDesc.colorAttachmentCount = frameBuffer.colorAttachment.size();
            renderPassDesc.colorAttachments = attachments;

            useBackbuffer = true;
        }
        else
        {
            renderPassDesc.colorAttachments = nullptr;
        }

        if (frameBuffer.depthAttachment.has_value())
        {
            auto depthTexture = frameBuffer.depthAttachment->texture;
            RUSH_ASSERT(depthTexture);
            depthStencilDesc.view = depthTexture->GetTexture().CreateView();
            depthStencilDesc.depthReadOnly = false;
            depthStencilDesc.stencilReadOnly = false;
            depthStencilDesc.depthClearValue = frameBuffer.clearDepth.has_value() ? frameBuffer.clearDepth.value() : 1.0f;
            depthStencilDesc.stencilClearValue = 0;
            depthStencilDesc.depthLoadOp = wgpu::LoadOp::Clear;
            depthStencilDesc.depthStoreOp = wgpu::StoreOp::Store;
            depthStencilDesc.stencilLoadOp = wgpu::LoadOp::Clear;
            depthStencilDesc.stencilStoreOp = wgpu::StoreOp::Store;
            renderPassDesc.depthStencilAttachment = &depthStencilDesc;

            useBackbuffer = true;
        }
        else
        {
            renderPassDesc.depthStencilAttachment = nullptr;
        }

        if (!useBackbuffer)
        {
            LOG_WARN("Framebuffer {} does not cantain any frame texutre", frameBuffer.lable);
        }

        wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderPassDesc);
        RUSH_ASSERT(renderQueue->camera.Valid());
        auto camera = renderQueue->camera.Get<Camera>();
        RUSH_ASSERT(camera);
        const auto& viewport = camera->GetViewport();        
        pass.SetViewport(viewport.x * m_Width, viewport.y * m_Height, (viewport.z - viewport.x) * m_Width, (viewport.w - viewport.y) * m_Height, 0.0f, 1.0f);
        for (const auto batch : renderQueue->m_Batches)
        {
            pass.SetPipeline(batch->pipeline->GetPipeline());
            pass.SetBindGroup(0, batch->bindGroup->GetBindGroup());
            int vbIdx = 0;
            for (auto vb : batch->vertexBuffers)
            {
                pass.SetVertexBuffer(vbIdx, vb->m_Buffer);
                ++vbIdx;
            }
            pass.SetIndexBuffer(batch->indexBuffer->m_Buffer, batch->indexBuffer->GetType());
            pass.DrawIndexed(batch->indexBuffer->GetIndexCount(), batch->instanceCount, 0, 0);
        }
        pass.End();
    }

    void Renderer::DrawQuad(const FrameBufferInfo& frameBuffer, Ref<RMaterialInst> material)
    {
        wgpu::RenderPassDescriptor renderPassDesc = {};
        renderPassDesc.label = frameBuffer.lable.c_str();
        wgpu::RenderPassColorAttachment attachment[8] = {};
        if (frameBuffer.colorAttachment.size() > 0)
        {
            for (int i = 0; i < frameBuffer.colorAttachment.size(); ++i)
            {
                attachment[i].view = frameBuffer.colorAttachment[i].texture->GetTexture().CreateView();
                attachment[i].resolveTarget = nullptr;
                attachment[i].loadOp = wgpu::LoadOp::Clear;
                attachment[i].storeOp = wgpu::StoreOp::Store;

                const auto& color = frameBuffer.colorAttachment[i].clearColor;
                attachment[i].clearValue = { color.r, color.g, color.b, color.a };
            }
            renderPassDesc.colorAttachmentCount = frameBuffer.colorAttachment.size();
            renderPassDesc.colorAttachments = attachment;
        }

        renderPassDesc.depthStencilAttachment = nullptr;

        wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderPassDesc);
        if (material)
        {
            pass.SetPipeline(material->GetMaterial()->GetPipeline()->GetPipeline());
            pass.SetBindGroup(0, material->GetBindGroup()->GetBindGroup());
            pass.SetVertexBuffer(0, m_QuadVB->m_Buffer);
            pass.Draw(4);
        }
        else
        {
            LOG_WARN("Frame buffer {} render quad does not have any material", frameBuffer.lable);
        }

        pass.End();
    }

    void Renderer::DrawSurface(RSurface& surface, const StringView& renderTexture)
    {
        wgpu::RenderPassDescriptor renderPassDesc = {};
        wgpu::RenderPassColorAttachment attachment = {};
        wgpu::TextureView backbufferView = surface.GetSwapChain().GetCurrentTextureView();
        attachment.view = backbufferView;
        attachment.resolveTarget = nullptr;
        attachment.loadOp = wgpu::LoadOp::Clear;
        attachment.storeOp = wgpu::StoreOp::Store;
        attachment.clearValue = { 0, 0, 0, 0 };
        renderPassDesc.colorAttachmentCount = 1;
        renderPassDesc.colorAttachments = &attachment;
        renderPassDesc.depthStencilAttachment = nullptr;

        wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderPassDesc);
        pass.SetPipeline(m_FinalPassPipeline->GetPipeline());
        pass.SetBindGroup(0, m_FinalPassBindGroup->GetBindGroup());
        pass.SetVertexBuffer(0, m_QuadVB->m_Buffer);
        pass.Draw(4);
        pass.End();
    }

    void Renderer::EndDraw()
    {
        wgpu::CommandBuffer commands = encoder.Finish();
        RDevice::instance().GetCmdQueue().Submit(1, &commands);
    }

    void Renderer::Resize(uint32_t width, uint32_t height)
    {
        m_Width = width;
        m_Height = height;
    }

    Ref<RTexture> Renderer::GetRenderTexture(const StringView& name) const
    {
        auto iter = m_RenderTextures.find(String(name));
        if (iter == m_RenderTextures.end())
        {
            Ref<RTexture> newTexture = CreateRef<RTexture>();
            return newTexture;
        }
        else
        {
            return iter->second;
        }
    }

}