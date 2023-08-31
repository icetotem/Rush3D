#ifndef RBuffer_h__
#define RBuffer_h__

#include "core/Core.h"
#include "render/RDefines.h"

namespace rush
{

    /// <summary>
    /// RBuffer
    /// </summary>
    class RVertexBuffer
    {
    public:
        RVertexBuffer(uint32_t stride, uint64_t size, const char* lable);

        ~RVertexBuffer() = default;

        void UpdateData(const void* data, uint64_t size, uint64_t startVertex = 0);

        uint64_t GetCount() const { return m_Count; }

        uint32_t GetStride() const { return m_Stride; }

        uint64_t GetSize() const { return m_Size; }

    protected:
        friend class Renderer;

        uint64_t m_Count = 0;
        uint32_t m_Stride = 0;
        uint64_t m_Size = 0;

        Ref<wgpu::Buffer> m_Buffer;
    };

    class RIndexBuffer
    {
    public:
        RIndexBuffer(uint64_t count, bool use32bits, const char* lable);

        ~RIndexBuffer() = default;

        void UpdateData(const void* data, uint64_t size, uint32_t startIndex = 0);

        bool Is32Bits() const { return m_Use32Bits; }

        uint64_t GetCount() const { return m_Count; }

        uint64_t GetSize() const { return m_Size; }

    protected:
        friend class Renderer;

        uint64_t m_Count = 0;
        uint64_t m_Size = 0;
        bool m_Use32Bits = 0;

        Ref<wgpu::Buffer> m_Buffer;
    };

}

#endif // RBuffer_h__
