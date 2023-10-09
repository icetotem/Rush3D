#include "stdafx.h"

#include <dawn/dawn_proc.h>

#include "render/Renderer.h"
#include "render/RShader.h"
#include "render/RDevice.h"
#include "components/Camera.h"
#include "components/Light.h"
#include "render/RMaterial.h"
#include "render/RGeometry.h"
#include "render/RTexture.h"
#include "render/RBindGroup.h"
#include "AssetManager.h"
#include "components/Transform.h"

namespace rush
{

    Renderer::Renderer(uint32_t width, uint32_t height)
    {
        m_Width = width;
        m_Height = height;
        CreateFullScreenQuad();

        AssetsManager::instance().LoadMaterial("assets/engine/pipeline/materials/final.mat", [&](AssetLoadResult result, Ref<RMaterial> material, void*) {
            if (result == AssetLoadResult::Success)
            {
                m_FinalPassMat = material;
            }
        });

        RegisterFGTexture("GBuffer0", wgpu::TextureFormat::BGRA8Unorm, Vector2(1.0f, 1.0f));
        RegisterFGTexture("GBuffer1", wgpu::TextureFormat::BGRA8Unorm, Vector2(1.0f, 1.0f));
        RegisterFGTexture("GBuffer2", wgpu::TextureFormat::BGRA8Unorm, Vector2(1.0f, 1.0f));
        RegisterFGTexture("GBuffer3", wgpu::TextureFormat::BGRA8Unorm, Vector2(1.0f, 1.0f));
        RegisterFGTexture("SceneDepthTexture", wgpu::TextureFormat::Depth24PlusStencil8, Vector2(1.0f, 1.0f));

        m_FrameDataBuffer = CreateRef<RUniformBuffer>(sizeof(m_FrameData), &m_FrameData, "FrameData_buffer");
        m_FrameDataGroup = CreateRef<RBindGroup>();
        m_FrameDataGroup->AddBinding(0, ShaderStage::Vertex | ShaderStage::Fragment, m_FrameDataBuffer);
        m_FrameDataGroup->Create("FrameData_group");

        m_DirectionalLightBuffer = CreateRef<RUniformBuffer>(sizeof(m_DirectionalLightData), &m_DirectionalLightData, "DirectionalLightData_buffer");
        m_PointLightsBuffer = CreateRef<RUniformBuffer>(sizeof(PointLightData) * kMaxPointLights, &m_PointLightsData, "PointLightData_buffer");
        m_LightingDataGroup = CreateRef<RBindGroup>();
        m_LightingDataGroup->AddBinding(0, ShaderStage::Fragment, m_DirectionalLightBuffer);
        m_LightingDataGroup->AddBinding(1, ShaderStage::Fragment, m_PointLightsBuffer);
        m_LightingDataGroup->Create("LightingData_group");
    }

    void Renderer::CreateFullScreenQuad()
    {
        float const verts[] =
        {
            -1.f,  3.f,   // BL
             3.f, -1.f,   // TR
            -1.f, -1.f,   // TL
        };

        VertexLayout vlayouts[kMaxVertexBuffers];
        vlayouts[0].attributeCount = 1;
        vlayouts[0].stride = 2 * sizeof(float);
        vlayouts[0].attributes[0].format = wgpu::VertexFormat::Float32x2;
        vlayouts[0].attributes[0].location = 0;
        vlayouts[0].attributes[0].offset = 0;
        m_ScreenQuadGeo = CreateRef<RGeometry>(PrimitiveTopology::TriangleStrip, 1, vlayouts, 3, 0, "FullSceenQuad");
        m_ScreenQuadGeo->UpdateVertexBuffer(0, verts, sizeof(verts));
    }

    void Renderer::RegisterFGTexture(const StringView& name, TextureFormat format, Vector2 viewScale)
    {
        auto iter = m_RenderTextures.find(String(name));
        if (iter == m_RenderTextures.end())
        {
            FrameTexture ft;
            ft.format = format;
            ft.scale = viewScale;
            m_RenderTextures.insert({ String(name), ft });
        }
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

        // set frame data
        if (m_FrameDataGroup->GetBindGroupHandle())
        {
            m_FrameData.proj = camera->GetProjMatrix();
            m_FrameData.view = camera->GetViewMatrix();
            m_FrameData.viewProj = m_FrameData.proj * m_FrameData.view;
            m_FrameDataBuffer->UpdateData(&m_FrameData, sizeof(m_FrameData));
            pass.SetBindGroup(0, m_FrameDataGroup->GetBindGroupHandle());
        }

        // set light data
        {
            m_PointLightsData.clear();
            for (auto light : renderQueue->GetLights())
            {
                if (light->GetType() == LightType::LT_Directional)
                {
                    m_DirectionalLightData.color = Vector4(light->GetColor(), light->GetIntensity());
                    m_DirectionalLightData.direction = light->Get<Transform>()->GetForward();
                }
                else if (light->GetType() == LightType::LT_Point || light->GetType() == LightType::LT_Spot)
                {
                    if (m_PointLightsData.size() < kMaxPointLights)
                    {
                        PointLightData& data = m_PointLightsData.emplace_back();
                        data.color = Vector4(light->GetColor(), light->GetIntensity());
                        data.position = light->Get<Transform>()->GetPosition();
                        data.radius = light->GetRaidus();
                        data.direction = light->Get<Transform>()->GetForward();
                        data.angle = light->GetSpotAngle();
                    }
                }
            }
            m_DirectionalLightData.pointLightCount = (float)m_PointLightsData.size();
            m_DirectionalLightBuffer->UpdateData(&m_DirectionalLightData, sizeof(m_DirectionalLightData));
            if (m_DirectionalLightData.pointLightCount > 0)
            {
                m_PointLightsBuffer->UpdateData(&m_PointLightsData[0], sizeof(PointLightData) * m_DirectionalLightData.pointLightCount);
            }
            pass.SetBindGroup(1, m_LightingDataGroup->GetBindGroupHandle());
        }

        for (const auto& batch : renderQueue->GetBatches())
        {
            auto geo = batch.renderable.geometry;
            auto mat = batch.renderable.material;
            pass.SetPipeline(RMaterial::GetPipeline(this, geo, mat, outputBuffers));
            if (mat->GetBindGroup() && mat->GetBindGroup()->GetBindGroupHandle())
                pass.SetBindGroup(2, mat->GetBindGroup()->GetBindGroupHandle());
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
                if (outputBuffers.colorAttachment[i].texture == kBackBufferName)
                {
                    wgpu::TextureView backbufferView = m_Surface->GetSwapChain().GetCurrentTextureView();
                    attachments[i].view = backbufferView;
                }
                else
                {
                    auto rt = GetFGTexture(outputBuffers.colorAttachment[i].texture);
                    if (rt == nullptr)
                    {
                        LOG_ERROR("Render texture {} is not registered", outputBuffers.colorAttachment[i].texture);
                        continue;
                    }
                    attachments[i].view = rt->GetTextureHandle().CreateView();
                }
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

        // set frame data
        //if (m_FrameDataGroup.GetBindGroupHandle())
        //    pass.SetBindGroup(0, m_FrameDataGroup.GetBindGroupHandle());

        if (material)
        {
            pass.SetPipeline(RMaterial::GetPipeline(this, m_ScreenQuadGeo, material, outputBuffers));
            if (material->GetBindGroup() && material->GetBindGroup()->GetBindGroupHandle())
                pass.SetBindGroup(0, material->GetBindGroup()->GetBindGroupHandle());
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
            info.lable = "Deferred Pass";
            info.colorAttachment.push_back({ "GBuffer0", GetFGTextureFormat("GBuffer0"), {0.2f, 0.2f, 0.2f, 1.0f} });
            info.colorAttachment.push_back({ "GBuffer1", GetFGTextureFormat("GBuffer1"), {0.2f, 0.2f, 0.2f, 1.0f} });
            info.colorAttachment.push_back({ "GBuffer2", GetFGTextureFormat("GBuffer2"), {0.2f, 0.2f, 0.2f, 1.0f} });
            info.colorAttachment.push_back({ "GBuffer3", GetFGTextureFormat("GBuffer3"), {0.2f, 0.2f, 0.2f, 1.0f} });
            info.depthStencilTexture = "SceneDepthTexture";
            info.depthStencilFormat = GetFGTextureFormat("SceneDepthTexture");
            info.clearDepth = 1.0f;
            DrawScene(renderQueue, info);
        }
        // final pass
        if (1)
        {
            FrameBuffer info;
            info.lable = "Final Pass";
            info.colorAttachment.push_back({ String(kBackBufferName), TextureFormat::BGRA8Unorm, {0.0f, 0.0f, 0.0f, 0.0f} });
            DrawQuad(m_FinalPassMat, info);
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
            Ref<RTexture> newTexture = CreateRef<RTexture>(m_Width * info.scale.x, m_Height * info.scale.y, info.format, 1, 1, TextureDimension::e2D, 
                                            TextureUsage::RenderAttachment | TextureUsage::TextureBinding, String(name).c_str());
            iter->second.texture = newTexture;
            return newTexture;
        }
        else
        {
            return iter->second.texture;
        }
    }

    TextureFormat Renderer::GetFGTextureFormat(const StringView& name)
    {
        auto iter = m_RenderTextures.find(String(name));
        if (iter == m_RenderTextures.end())
        {
            return TextureFormat::Undefined;
        }
        else if (iter->second.texture == nullptr)
        {
            const FrameTexture& info = iter->second;
            return info.format;
        }
        else
        {
            return iter->second.format;
        }
    }

    Vector2 Renderer::GetFGTextureScale(const StringView& name)
    {
        auto iter = m_RenderTextures.find(String(name));
        if (iter == m_RenderTextures.end())
        {
            return Vector2(0);
        }
        else if (iter->second.texture == nullptr)
        {
            const FrameTexture& info = iter->second;
            return info.scale;
        }
        else
        {
            return iter->second.scale;
        }
    }

}