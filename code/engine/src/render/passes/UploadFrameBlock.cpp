#include "stdafx.h"
#include "render/passes/UploadFrameBlock.h"
#include "render/fg/FrameGraph.hpp"
#include "render/fg/Blackboard.hpp"
#include "render/passes/FrameGraphBuffer.h"
#include "render/passes/FrameGraphHelper.h"
#include "render/passes/PassesData.h"
#include "components/Camera.h"

namespace {

    struct alignas(16) GPUCamera 
    {
        GPUCamera(const rush::Camera& camera)
            : projection{ camera.GetProjMatrix() },
            inversedProjection{ glm::inverse(projection) }, 
            view{ camera.GetViewMatrix() },
            inversedView{ glm::inverse(view) }, 
            fov{ camera.GetFov() },
            _near{ camera.GetNearClip() }, 
            _far{ camera.GetFarClip() } {}

    private:
        rush::Matrix4 projection{ 1.0f };
        rush::Matrix4 inversedProjection{1.0f};
        rush::Matrix4 view{1.0f};
        rush::Matrix4 inversedView{1.0f};
        float fov;
        float _near, _far;
        // Implicit padding, 4bytes
    };

    struct GPUFrameBlock 
    {
        float time{ 0.0f };
        float deltaTime{ 0.0f };
        rush::IVector2 resolution{ 0 };
        GPUCamera camera;
        uint32_t renderFeatures{ 0 };
        uint32_t debugFlags{ 0 };
    };

} // namespace

namespace rush
{

    void uploadFrameBlock(FrameGraph& fg, FrameGraphBlackboard& blackboard, const FrameInfo& frameInfo) 
    {
        blackboard.add<FrameData>() = fg.addCallbackPass<FrameData>(
            "UploadFrameBlock",
                [&](FrameGraph::Builder& builder, FrameData& data) {
                data.frameBlock = builder.create<FrameGraphBuffer>( "FrameBlock", { sizeof(GPUFrameBlock) });
                data.frameBlock = builder.write(data.frameBlock);
            },
            [=](const FrameData& data, FrameGraphPassResources& resources, void* ctx) {
    //             NAMED_DEBUG_MARKER("UploadFrameBlock");
    //             TracyGpuZone("UploadFrameBlock");

                const GPUFrameBlock frameBlock
                {
                    frameInfo.time,
                    frameInfo.deltaTime,
                    frameInfo.resolution,
                    GPUCamera{frameInfo.camera},
                    frameInfo.features,
                    frameInfo.debugFlags,
                };

                auto& buffer = getBuffer(resources, data.frameBlock);
                buffer.UpdateData(&frameBlock);
            });
    }

}