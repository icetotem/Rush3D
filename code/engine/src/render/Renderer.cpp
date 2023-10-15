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

#define MAX_INSTANCE_COUNT 2000

    Renderer::Renderer(uint32_t width, uint32_t height)
    {
        m_Width = width;
        m_Height = height;
        CreateFullScreenQuad();

        m_FinalPassMat = AssetsManager::instance().GetMaterial("assets/engine/pipeline/final.mat");
        m_DeferredLightingPassMat = AssetsManager::instance().GetMaterial("assets/engine/pipeline/deferred_lighting.mat");

        RegisterFGRenderTexture("GBuffer0", TextureFormat::BGRA8Unorm, TextureViewDimension::e2D, Vector2(1.0f, 1.0f));
        RegisterFGRenderTexture("GBuffer1", TextureFormat::BGRA8Unorm, TextureViewDimension::e2D, Vector2(1.0f, 1.0f));
        RegisterFGRenderTexture("GBuffer2", TextureFormat::BGRA8Unorm, TextureViewDimension::e2D, Vector2(1.0f, 1.0f));
        RegisterFGRenderTexture("GBuffer3", TextureFormat::BGRA8Unorm, TextureViewDimension::e2D, Vector2(1.0f, 1.0f));
        RegisterFGRenderTexture("SceneDepth", TextureFormat::Depth24Plus, TextureViewDimension::e2D, Vector2(1.0f, 1.0f));
        RegisterFGRenderTexture("LDR", TextureFormat::RGBA16Float, TextureViewDimension::e2D, Vector2(1.0f, 1.0f));
        RegisterFGRenderTexture("SSAO", TextureFormat::RGBA16Float, TextureViewDimension::e2D, Vector2(1.0f, 1.0f));
        RegisterFGRenderTexture("BRDF", TextureFormat::RGBA16Float, TextureViewDimension::e2D, Vector2(1.0f, 1.0f));
        RegisterFGDynamicTexture("IrradianceMap", TextureFormat::RGBA16Float, TextureViewDimension::Cube, Vector2(1.0f, 1.0f));
        RegisterFGDynamicTexture("PrefilteredEnvMap", TextureFormat::RGBA16Float, TextureViewDimension::Cube, Vector2(1.0f, 1.0f));

        m_FrameDataBuffer = CreateRef<RUniformBuffer>(sizeof(m_FrameData), &m_FrameData, "FrameData_buffer");
        m_FrameDataGroup = CreateRef<RBindGroup>();
        m_FrameDataGroup->AddBinding(0, ShaderStage::Vertex | ShaderStage::Fragment, m_FrameDataBuffer, wgpu::BufferBindingType::Uniform);
        m_FrameDataGroup->Create("FrameData_group");

        m_TransformBuffer = CreateRef<RStorageBuffer>(sizeof(Matrix4) * MAX_INSTANCE_COUNT, nullptr, "Transform_buffer");
        m_TransformDataGroup = CreateRef<RBindGroup>();
        m_TransformDataGroup->AddBinding(0, ShaderStage::Vertex, m_TransformBuffer, wgpu::BufferBindingType::ReadOnlyStorage);
        m_TransformDataGroup->Create("Transform_group");

        for (int i = 0; i < MAX_INSTANCE_COUNT; ++i)
        {
            auto& instBindGroup = m_InstanceBindGroups.emplace_back();
            instBindGroup.buffer = CreateRef<RUniformBuffer>(sizeof(InstanceData), &instBindGroup.data, "InstanceData_buffer");
            instBindGroup.group = CreateRef<RBindGroup>();
            instBindGroup.group->AddBinding(0, ShaderStage::Vertex, instBindGroup.buffer, wgpu::BufferBindingType::Uniform);
            instBindGroup.group->Create("InstanceData_group");
        }

        m_LightsBuffer = CreateRef<RStorageBuffer>(sizeof(m_LightsData), &m_LightsData, "LightData_buffer");
        m_LightDataGroup = CreateRef<RBindGroup>();
        m_LightDataGroup->AddBinding(0, ShaderStage::Fragment, m_LightsBuffer, wgpu::BufferBindingType::ReadOnlyStorage);
        m_LightDataGroup->Create("LightData_group");
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

                wgpu::TextureViewDescriptor tvdesc = {};
                tvdesc.dimension = GetFGTextureViewDim(outputBuffers.colorAttachment[i].texture);
                attachments[i].view = rt->GetTextureHandle().CreateView(&tvdesc);
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
                depthStencilDesc.depthClearValue = outputBuffers.clearDepth.has_value() ? outputBuffers.clearDepth.value() : 1.0f;
                depthStencilDesc.depthLoadOp = wgpu::LoadOp::Clear;
                depthStencilDesc.depthStoreOp = wgpu::StoreOp::Store;

                if (outputBuffers.clearStencil.has_value())
                {
                    depthStencilDesc.stencilReadOnly = false;
                    depthStencilDesc.stencilClearValue = outputBuffers.clearStencil.value();
                    depthStencilDesc.stencilLoadOp = wgpu::LoadOp::Clear;
                    depthStencilDesc.stencilStoreOp = wgpu::StoreOp::Store;
                }

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
            m_FrameData.camera.projection = camera->GetProjMatrix();
            m_FrameData.camera.view = camera->GetViewMatrix();
            m_FrameData.camera.inversedProjection = glm::inverse(camera->GetProjMatrix());
            m_FrameData.camera.inversedView = glm::inverse(camera->GetViewMatrix());
            m_FrameData.camera.vp = m_FrameData.camera.projection * m_FrameData.camera.view;
            m_FrameData.camera.fov = camera->GetFov();
            m_FrameData.camera._near = camera->GetNearClip();
            m_FrameData.camera._far = camera->GetFarClip();
            m_FrameData.time = Timer::GetTimeSec();
            m_FrameData.deltaTime = Timer::GetDeltaTimeSec();
            m_FrameData.resolution = IVector2(m_Width, m_Height);
            m_FrameData.renderFeatures = 0;
            m_FrameData.deltaTime = 0;
            m_FrameDataBuffer->UpdateData(&m_FrameData, sizeof(m_FrameData));
            pass.SetBindGroup(0, m_FrameDataGroup->GetBindGroupHandle());
        }

        // set transform data
        m_Transforms.clear();
        if (m_TransformDataGroup->GetBindGroupHandle())
        {
            for (const auto& batch : renderQueue->GetBatches())
            {
                for (uint32_t i = 0; i < batch->instanceCount; ++i)
                {
                    m_Transforms.push_back(*batch->transforms[i]);
                }
            }
            m_TransformBuffer->UpdateData(m_Transforms.data(), sizeof(Matrix4) * m_Transforms.size());
            pass.SetBindGroup(2, m_TransformDataGroup->GetBindGroupHandle());
        }

        // update light data
        if (m_LightDataGroup->GetBindGroupHandle())
        {
            m_LightsData.numLights = renderQueue->GetLights().size();
            int lt = 0;
            for (auto light : renderQueue->GetLights())
            {
                m_LightsData.data[lt].color = Vector4(light->GetColor(), light->GetIntensity());
                m_LightsData.data[lt].direction = light->Get<Transform>()->GetForward();
                m_LightsData.data[lt].range = light->GetRaidus();
                m_LightsData.data[lt].innerConeAngle = light->GetInnerAngle();
                m_LightsData.data[lt].outerConeAngle = light->GetOutterAngle();
                m_LightsData.data[lt].type = (uint32_t)light->GetType();
                m_LightsData.data[lt].position = light->Get<Transform>()->GetPosition();
                ++lt;
            }
            m_LightsBuffer->UpdateData(&m_LightsData, m_LightsData.numLights * sizeof(LightData) + 16);
        }

        uint32_t transformOffset = 0;
        uint32_t batchIndex = 0;
        uint64_t lastPipeHash = 0;
        uint64_t lastGroupHash = 0;
        for (auto batch : renderQueue->GetBatches())
        {
            auto geo = batch->renderable.geometry;
            auto mat = batch->renderable.material;
            auto pipe = RMaterial::GetPipeline(this, geo, mat, outputBuffers);

            if (lastPipeHash != batch->renderable.hashPipeline)
            {
                pass.SetPipeline(pipe);
                lastPipeHash = batch->renderable.hashPipeline;

                if (mat && mat->GetBindGroup() && mat->GetBindGroup()->GetBindGroupHandle() && mat->GetBindGroupHash() != lastGroupHash)
                {
                    lastGroupHash = mat->GetBindGroupHash();
                    pass.SetBindGroup(1, mat->GetBindGroup()->GetBindGroupHandle());
                }
            }

            if (m_InstanceBindGroups.size() < batchIndex + 1)
            {
                auto& instBindGroup = m_InstanceBindGroups.emplace_back();
                instBindGroup.buffer = CreateRef<RUniformBuffer>(sizeof(InstanceData), &instBindGroup.data, "InstanceData_buffer");
                instBindGroup.group = CreateRef<RBindGroup>();
                instBindGroup.group->AddBinding(0, ShaderStage::Vertex, instBindGroup.buffer, wgpu::BufferBindingType::Uniform);
                instBindGroup.group->Create("InstanceData_group");
            }

            m_InstanceBindGroups[batchIndex].data.transformOffset = transformOffset;
            m_InstanceBindGroups[batchIndex].buffer->UpdateData(&m_InstanceBindGroups[batchIndex].data);
            transformOffset += batch->instanceCount;
            pass.SetBindGroup(3, m_InstanceBindGroups[batchIndex].group->GetBindGroupHandle());

            for (uint32_t vb = 0; vb < geo->GetVBCount(); ++vb)
            {
                pass.SetVertexBuffer(vb, geo->GetVB(vb)->GetBufferHandle());
            }
            pass.SetIndexBuffer(geo->GetIB()->GetBufferHandle(), geo->GetIB()->GetType());
            pass.DrawIndexed(geo->GetIB()->GetIndexCount(), batch->instanceCount, 0, 0);

            ++batchIndex;
        }
        pass.End();
    }

    void Renderer::DrawQuad(Ref<RMaterial> material, const FrameBuffer& outputBuffers)
    {
        if (material == nullptr)
        {
            LOG_WARN("Frame buffer {} render quad does not have any material", outputBuffers.lable);
            return;
        }

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
                    wgpu::TextureViewDescriptor tvdesc = {};
                    tvdesc.dimension = GetFGTextureViewDim(outputBuffers.colorAttachment[i].texture);
                    attachments[i].view = rt->GetTextureHandle().CreateView(&tvdesc);
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
        if (m_FrameDataGroup->GetBindGroupHandle())
            pass.SetBindGroup(0, m_FrameDataGroup->GetBindGroupHandle());

        pass.SetPipeline(RMaterial::GetPipeline(this, m_ScreenQuadGeo, material, outputBuffers));
        if (material->GetBindGroup() && material->GetBindGroup()->GetBindGroupHandle())
        {
            pass.SetBindGroup(1, material->GetBindGroup()->GetBindGroupHandle());
        }

        const auto& globalGroups = material->GetGlobalBindGroups();
        int bgIndex = 2;
        for (const auto& bg : globalGroups)
        {
            if (bg == "light_data")
            {
                pass.SetBindGroup(bgIndex, GetLightDataGroup()->GetBindGroupHandle());
            }
            else if (bg == "transform_data")
            {
                pass.SetBindGroup(bgIndex, GetTransformDataGroup()->GetBindGroupHandle());
            }
            else if (bg == "instance_data")
            {
                pass.SetBindGroup(bgIndex, GetInstanceDataGroup()->GetBindGroupHandle());
            }
            ++bgIndex;
        }

        pass.SetVertexBuffer(0, m_ScreenQuadGeo->GetVB(0)->GetBufferHandle());
        pass.Draw(3);

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

        // g-buffer pass
        if (1)
        {
            FrameBuffer info;
            info.lable = "g-buffer Pass"; 
            GenAttachment(info.colorAttachment.emplace_back(), "GBuffer0", Vector4(0));
            GenAttachment(info.colorAttachment.emplace_back(), "GBuffer1", Vector4(0.2f, 0.2f, 0.2f, 0.0f)); // only albedo can with clear color
            GenAttachment(info.colorAttachment.emplace_back(), "GBuffer2", Vector4(0));
            GenAttachment(info.colorAttachment.emplace_back(), "GBuffer3", Vector4(0));
            info.depthStencilTexture = "SceneDepth";
            info.depthStencilFormat = GetFGTextureFormat("SceneDepth");
            info.clearDepth = 1.0f;
            DrawScene(renderQueue, info);
        }

        // deferred lighting pass
        if (1)
        {
            FrameBuffer info;
            info.lable = "Deferred Lighting Pass";
            GenAttachment(info.colorAttachment.emplace_back(), "LDR", Vector4(0));
            DrawQuad(m_DeferredLightingPassMat, info);
        }

        // final pass
        if (1)
        {
            FrameBuffer info;
            info.lable = "Final Pass";
            GenAttachment(info.colorAttachment.emplace_back(), kBackBufferName, Vector4(0));
            DrawQuad(m_FinalPassMat, info);
        }

        EndDraw();
    }

    void Renderer::Resize(uint32_t width, uint32_t height)
    {
        m_Width = width;
        m_Height = height;
    }

    void Renderer::RegisterFGRenderTexture(const StringView& name, TextureFormat format, TextureViewDimension dim, Vector2 viewScale)
    {
        auto iter = m_RenderTextures.find(String(name));
        if (iter == m_RenderTextures.end())
        {
            SceneGraphTexture ft;
            ft.format = format;
            ft.size = viewScale;
            ft.dim = dim;
            ft.isRT = true;
            m_RenderTextures.insert({ String(name), ft });
        }
    }

    void Renderer::RegisterFGDynamicTexture(const StringView& name, TextureFormat format, TextureViewDimension dim, Vector2 size)
    {
        auto iter = m_RenderTextures.find(String(name));
        if (iter == m_RenderTextures.end())
        {
            SceneGraphTexture ft;
            ft.format = format;
            ft.size = size;
            ft.dim = dim;
            ft.isRT = false;
            m_RenderTextures.insert({ String(name), ft });
        }
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
            const SceneGraphTexture& info = iter->second;
            int layer = 1;
            if (info.dim == wgpu::TextureViewDimension::Cube)
            {
                layer = 6;
            }
            uint32_t width, height;
            if (iter->second.isRT)
            {
                width = m_Width * info.size.x;
                height = m_Height * info.size.y;
            }
            else
            {
                width = info.size.x;
                height = info.size.y;
            }
            Ref<RTexture> newTexture = CreateRef<RTexture>(width, height, info.format, 1, layer, TextureDimension::e2D,
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
        if (name == kBackBufferName)
        {
            return TextureFormat::BGRA8Unorm;
        }
        else
        {
            auto iter = m_RenderTextures.find(String(name));
            if (iter == m_RenderTextures.end())
            {
                return TextureFormat::Undefined;
            }
            else if (iter->second.texture == nullptr)
            {
                const SceneGraphTexture& info = iter->second;
                return info.format;
            }
            else
            {
                return iter->second.format;
            }
        }
    }

    TextureViewDimension Renderer::GetFGTextureViewDim(const StringView& name)
    {
        auto iter = m_RenderTextures.find(String(name));
        if (iter == m_RenderTextures.end())
        {
            return TextureViewDimension::e2D;
        }
        else if (iter->second.texture == nullptr)
        {
            const SceneGraphTexture& info = iter->second;
            return info.dim;
        }
        else
        {
            return iter->second.dim;
        }
    }

}