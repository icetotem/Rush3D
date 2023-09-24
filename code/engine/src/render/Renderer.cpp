#include "stdafx.h"

#include <dawn/dawn_proc.h>

#include "render/Renderer.h"
#include "render/RShader.h"
#include "render/RDevice.h"
#include "components/Camera.h"
#include "render/RPipeline.h"


namespace rush
{

    Renderer::Renderer(uint32_t width, uint32_t height)
    {
        m_Width = width;
        m_Height = height;
        
        //CreateFullScreenQuad();

        RegisterFGTexture("SceneColorTexture", TextureFormat::BGRA8Unorm, 1.0f, 1.0f);
        RegisterFGTexture("SceneDepthTexture", TextureFormat::Depth24PlusStencil8, 1.0f, 1.0f);
    }

    void Renderer::CreateFullScreenQuad()
    {
//         const char screen_quad_vs[] = R"(
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
// 	            })";
// 
//         m_QuadVS = CreateRef<RShader>(ShaderStage::Vertex, screen_quad_vs, "screen_quad_vs");
// 
//         const char final_quad_fs[] = R"(
//         	    @group(0) @binding(0) var mySampler : sampler;
// 			    @group(0) @binding(1) var myTexture : texture_2d<f32>;
// 	            @fragment
// 	            fn main(@location(0) vUV : vec2<f32>) -> @location(0) vec4<f32> {
// 		            var color = textureSample(myTexture, mySampler, vUV);
//                     return color;
// 	            })";
// 
//         m_QuadFSFinal = CreateRef<RShader>(ShaderStage::Vertex, final_quad_fs, "final_quad_fs");

        float const verts[] =
        {
            -1.f, -1.f,   // TL
             3.f, -1.f,   // TR
            -1.f,  3.f,   // BL
        };

        //m_QuadVB = CreateRef<RVertexBuffer>(sizeof(float) * 2, sizeof(verts), verts, "screen_quad_vb");

        VertexLayout vlayouts[kMaxVertexBuffers];
        vlayouts[0].attribteCount = 1;
        vlayouts[0].stride = 2 * sizeof(float);
        vlayouts[0].attributes[0].format = wgpu::VertexFormat::Float32x2;
        vlayouts[0].attributes[0].location = 0;
        vlayouts[0].attributes[0].offset = 0;
        m_ScreenQuadGeo = CreateRef<RGeometry>(PrimitiveTopology::TriangleStrip, 1, vlayouts, 3, 0, "FullSceenQuad");
        m_ScreenQuadGeo->UpdateVertexBuffer(0, verts, sizeof(verts));

//         PipelineDesc pipeDesc = {};
//         pipeDesc.depthWrite = false;
//         pipeDesc.depthTest = false;
//         pipeDesc.colorFormat = TextureFormat::BGRA8Unorm;
// 
//         VertexAttribute vertAttr;
//         vertAttr.format = VertexFormat::Float32x2;
//         vertAttr.offset = 0;
//         vertAttr.shaderLocation = 0;
// 
//         auto& vLayout0 = pipeDesc.vertexLayouts.emplace_back();
//         vLayout0.stride = sizeof(float) * 2;
//         vLayout0.attributes = &vertAttr;
//         vLayout0.attributeCount = 1;
// 
//         auto l = {
//             BindingLayoutHelper(0, ShaderStage::Fragment, SamplerBindingType::Filtering),
//             BindingLayoutHelper(1, ShaderStage::Fragment, TextureSampleType::Float, TextureViewDimension::e2D)
//         };
//         Ref<BindingLayout> bindingLayout = CreateRef<BindingLayout>(l);
// 
//         pipeDesc.vs = m_QuadVS;
//         pipeDesc.fs = m_QuadFSFinal;
//         pipeDesc.writeMask = ColorWriteMask::All;
//         pipeDesc.bindLayout = bindingLayout;
//         pipeDesc.primitiveType = PrimitiveTopology::TriangleList;
//         pipeDesc.frontFace = FrontFace::CCW;
//         pipeDesc.cullModel = CullMode::Back;
//         m_FinalPassPipeline = CreateRef<RPipeline>(pipeDesc);
// 
//         Ref<RTexture> tex = CreateRef<RTexture>(m_Width, m_Height, TextureFormat::RGBA8Unorm, 1, 1, TextureDimension::e2D, TextureUsage::RenderAttachment, "FinalPass Texture Buffer");
//         auto layout = {
//             BindingInitializationHelper(0, CreateRef<RSampler>()),
//             BindingInitializationHelper(1, tex)
//         };
//         m_FinalPassBindGroup = CreateRef<RBindGroup>(bindingLayout, layout, "FinalPass BindGroup");
    }


    void Renderer::RegisterFGTexture(const StringView& name, TextureFormat format, float widthScale, float heightScale)
    {
        FrameTexture ft;
        ft.format = format;
        ft.widthScale = widthScale;
        ft.heightScale = heightScale;
        m_RenderTextures.insert({String(name), ft});
    }

    void Renderer::BeginDraw(Ref<RSurface> surface)
    {
        dawn::native::InstanceProcessEvents(RDevice::instance().GetDawnInstance().Get());
        encoder = RDevice::instance().GetDevice().CreateCommandEncoder();
        m_Surface = surface;
    }

    void Renderer::DrawScene(Ref<RenderQueue> renderQueue, const FrameBuffer& outputBuffers)
    {
        wgpu::RenderPassDescriptor renderPassDesc = {};
        renderPassDesc.label = outputBuffers.lable.c_str();

        wgpu::RenderPassColorAttachment attachments[8] = {};
        wgpu::RenderPassDepthStencilAttachment depthStencilDesc;

        bool useBackbuffer = false;
        if (outputBuffers.colorAttachment.size() > 0)
        {
            for (int i = 0; i < outputBuffers.colorAttachment.size(); ++i)
            {
                auto rt = GetFGTexture(outputBuffers.colorAttachment[i].texture);
                if (rt == nullptr)
                {
                    LOG_ERROR("Render texture {} is not registered", outputBuffers.colorAttachment[i].texture);
                    continue;
                }
                attachments[i].view = rt->GetTextureHandle().CreateView();
                attachments[i].resolveTarget = nullptr;
                attachments[i].loadOp = wgpu::LoadOp::Clear;
                attachments[i].storeOp = wgpu::StoreOp::Store;

                const auto& color = outputBuffers.colorAttachment[i].clearColor;
                attachments[i].clearValue = { color.r, color.g, color.b, color.a };
            }
            renderPassDesc.colorAttachmentCount = outputBuffers.colorAttachment.size();
            renderPassDesc.colorAttachments = attachments;

            useBackbuffer = true;
        }
        else
        {
            renderPassDesc.colorAttachments = nullptr;
        }

        if (outputBuffers.depthStencilTexture.has_value())
        {
            auto rt = GetFGTexture(outputBuffers.depthStencilTexture.value());
            if (rt != nullptr)
            {
                depthStencilDesc.view = rt->GetTextureHandle().CreateView();
                depthStencilDesc.depthReadOnly = false;
                depthStencilDesc.stencilReadOnly = false;
                depthStencilDesc.depthClearValue = outputBuffers.clearDepth.has_value() ? outputBuffers.clearDepth.value() : 1.0f;
                depthStencilDesc.stencilClearValue = outputBuffers.clearStencil.has_value() ? outputBuffers.clearStencil.value() : 0.0f;
                depthStencilDesc.depthLoadOp = wgpu::LoadOp::Clear;
                depthStencilDesc.depthStoreOp = wgpu::StoreOp::Store;
                depthStencilDesc.stencilLoadOp = wgpu::LoadOp::Clear;
                depthStencilDesc.stencilStoreOp = wgpu::StoreOp::Store;
                renderPassDesc.depthStencilAttachment = &depthStencilDesc;
            }
            else
            {
                LOG_ERROR("Render texture {} is not registered", outputBuffers.depthStencilTexture.value());
            }

            useBackbuffer = true;
        }
        else
        {
            renderPassDesc.depthStencilAttachment = nullptr;
        }

        if (!useBackbuffer)
        {
            wgpu::TextureView backbufferView = m_Surface->GetSwapChain().GetCurrentTextureView();
            attachments[0].view = backbufferView;
            attachments[0].resolveTarget = nullptr;
            attachments[0].loadOp = wgpu::LoadOp::Clear;
            attachments[0].storeOp = wgpu::StoreOp::Store;
            attachments[0].clearValue = { 0, 0, 0, 0 };
            renderPassDesc.colorAttachmentCount = 1;
            renderPassDesc.colorAttachments = attachments;
        }
        else if (m_Surface == nullptr)
        {
            LOG_WARN("Framebuffer {} does not assign any render target", outputBuffers.lable);
            return;
        }

        wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderPassDesc);
        RUSH_ASSERT(renderQueue->GetCamera().Valid());
        auto camera = renderQueue->GetCamera().Get<Camera>();
        RUSH_ASSERT(camera);
        const auto& viewport = camera->GetViewport();        
        pass.SetViewport(viewport.x * m_Width, viewport.y * m_Height, (viewport.z - viewport.x) * m_Width, (viewport.w - viewport.y) * m_Height, 0.0f, 1.0f);
        for (const auto& batch : renderQueue->GetBatches())
        {
            auto geo = batch.renderable.geometry;
            auto mat = batch.renderable.material;
            pass.SetPipeline(RMaterial::GetPipeline(geo, mat));
            pass.SetBindGroup(0, mat->GetBindGroup());
            for (uint32_t vb = 0; vb < geo->GetVBCount(); ++vb)
            {
                pass.SetVertexBuffer(vb, geo->GetVB(vb)->GetBufferHandle());
            }
            pass.SetIndexBuffer(geo->GetIB()->GetBufferHandle(), geo->GetIB()->GetType());
            pass.DrawIndexed(geo->GetIB()->GetIndexCount(), batch.instanceCount, 0, 0);
        }
        pass.End();
    }

    void Renderer::DrawQuad(Ref<RMaterial> material, const FrameBuffer& outputBuffers)
    {
        wgpu::RenderPassDescriptor renderPassDesc = {};
        renderPassDesc.label = outputBuffers.lable.c_str();
        renderPassDesc.depthStencilAttachment = nullptr;

        wgpu::RenderPassColorAttachment attachments[8] = {};
        bool useBackbuffer = false;
        if (outputBuffers.colorAttachment.size() > 0)
        {
            for (int i = 0; i < outputBuffers.colorAttachment.size(); ++i)
            {
                auto rt = GetFGTexture(outputBuffers.colorAttachment[i].texture);
                if (rt == nullptr)
                {
                    LOG_ERROR("Render texture {} is not registered", outputBuffers.colorAttachment[i].texture);
                    continue;
                }
                attachments[i].view = rt->GetTextureHandle().CreateView();
                attachments[i].resolveTarget = nullptr;
                attachments[i].loadOp = wgpu::LoadOp::Clear;
                attachments[i].storeOp = wgpu::StoreOp::Store;

                const auto& color = outputBuffers.colorAttachment[i].clearColor;
                attachments[i].clearValue = { color.r, color.g, color.b, color.a };

                useBackbuffer = true;
            }
            renderPassDesc.colorAttachmentCount = outputBuffers.colorAttachment.size();
            renderPassDesc.colorAttachments = attachments;
        }

        if (!useBackbuffer)
        {
            wgpu::TextureView backbufferView = m_Surface->GetSwapChain().GetCurrentTextureView();
            attachments[0].view = backbufferView;
            attachments[0].resolveTarget = nullptr;
            attachments[0].loadOp = wgpu::LoadOp::Clear;
            attachments[0].storeOp = wgpu::StoreOp::Store;
            attachments[0].clearValue = { 0, 0, 0, 0 };
            renderPassDesc.colorAttachmentCount = 1;
            renderPassDesc.colorAttachments = attachments;
        }

        wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderPassDesc);
        if (material)
        {
            pass.SetPipeline(RMaterial::GetPipeline(m_ScreenQuadGeo, material));
            pass.SetBindGroup(0, material->GetBindGroup());
            pass.SetVertexBuffer(0, m_ScreenQuadGeo->GetVB(0)->GetBufferHandle());
            pass.Draw(3);
        }
        else
        {
            LOG_WARN("Frame buffer {} render quad does not have any material", outputBuffers.lable);
        }

        pass.End();
    }

    void Renderer::EndDraw()
    {
        wgpu::CommandBuffer commands = encoder.Finish();
        RDevice::instance().GetCmdQueue().Submit(1, &commands);
    }

    void Renderer::Render(Ref<RenderQueue> renderQueue, Ref<RSurface> surface)
    {
        BeginDraw(surface);

        // forward pass
        if (1)
        {
            FrameBuffer info;
            info.lable = "Forward Pass";
            info.colorAttachment.push_back({ "SceneColorTexture", {0.1f, 0.1f, 0.1f, 1.0f} });
            FrameBufferAttachment depth;
            info.depthStencilTexture = "SceneDepthTexture";
            info.clearDepth = 1.0f;
            DrawScene(renderQueue, info);
        }
        // final pass
        if (1)
        {
            FrameBuffer info;

            DrawQuad(nullptr, info);          
        }        

        EndDraw();
    }

    void Renderer::Resize(uint32_t width, uint32_t height)
    {
        m_Width = width;
        m_Height = height;
    }

    Ref<RTexture> Renderer::GetFGTexture(const StringView& name)
    {
        auto iter = m_RenderTextures.find(String(name));
        if (iter == m_RenderTextures.end())
        {
            return nullptr;
        }
        else if (iter->second.texture == nullptr)
        {
            const FrameTexture& info = iter->second;
            Ref<RTexture> newTexture = CreateRef<RTexture>(m_Width * info.widthScale, m_Height * info.heightScale, info.format, 1, 1, TextureDimension::e2D, TextureUsage::RenderAttachment, String(name).c_str());
            iter->second.texture = newTexture;
            return newTexture;
        }
        else
        {
            return iter->second.texture;
        }
    }

}