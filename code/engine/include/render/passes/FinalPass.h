#ifndef FinalPass_h__
#define FinalPass_h__

#include "render/RPipeline.h"
#include "render/fg/FrameGraph.hpp"
#include "render/fg/Blackboard.hpp"

namespace rush
{
    enum class OutputMode : uint32_t 
    {
        Depth = 0,
        Emissive,
        BaseColor,
        Normal,
        Metallic,
        Roughness,
        AmbientOcclusion,

        SSAO,
        BrightColor,
        Reflections,

        Accum,
        Reveal,

        LightHeatmap,

        HDR,
        FinalImage,
    };

    class FinalPass 
    {
    public:
        FinalPass();
        ~FinalPass();

        void compose(FrameGraph&, const FrameGraphBlackboard&, OutputMode);

    private:
        Ref<RPipeline> m_pipeline;
    };
}

#endif // FinalPass_h__
