#ifndef MeshFilter_h__
#define MeshFilter_h__

#include "components/EcsSystem.h"

namespace rush
{
    class MeshFilter : public Component
    {
    public:        
        MeshFilter(Entity owner) : Component(owner) {}

        int AddPart(const StringView& meshPath, const StringView& matPath);
        void RemovePart(int slot);

    protected:
    };
}

#endif // MeshFilter_h__
