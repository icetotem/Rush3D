#ifndef RGeometry_h__
#define RGeometry_h__

#include "core/Core.h"
#include "RBuffer.h"
#include "render/RDefines.h"

namespace rush
{

    struct VertexAttr
    {
        VertexFormat format;
        uint64_t offset = 0;
        uint32_t location = 0;
    };

    struct VertexLayout
    {
        uint64_t stride;
        uint32_t attributeCount;
        VertexAttr attributes[kMaxVertexBuffers];
    };
    
    class RGeometry
    {
    public:
        RGeometry(PrimitiveTopology type, uint32_t bufferCount, const VertexLayout* layouts, uint64_t vertCount, uint64_t indexCount, const StringView& lable = "");
        ~RGeometry() = default;

        void UpdateVertexBuffer(uint32_t index, const void* data, uint64_t size, uint64_t offset = 0);
        void UpdateIndexBuffer(const void* data, uint64_t size, uint64_t offset = 0);

        PrimitiveTopology GetPrimitiveType() const { return m_Type; }
        uint32_t GetVBCount() const { return m_BufferCount; }
        Ref<RVertexBuffer> GetVB(int index) { return m_VertexBuffers[index]; }
        Ref<RIndexBuffer> GetIB() { return m_IndexBuffer; }

        uint64_t GetLayoutHash() const { return m_LayoutHash; }

        const VertexLayout* GetLayouts() const { return m_Layouts; }

    protected:
        PrimitiveTopology m_Type = PrimitiveTopology::TriangleList;
        DArray<Ref<RVertexBuffer>> m_VertexBuffers;
        Ref<RIndexBuffer> m_IndexBuffer;
        uint32_t m_BufferCount = 0;
        VertexLayout m_Layouts[kMaxVertexBuffers];
        uint64_t m_LayoutHash = 0;
    };

}

#endif // RGeometry_h__
