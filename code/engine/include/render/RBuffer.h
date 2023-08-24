#pragma once

#include "core/Core.h"
#include "render/RenderDefines.h"

namespace rush
{

    /// <summary>
    /// RBuffer
    /// </summary>
    class RVertexBuffer
    {
    public:
        ~RVertexBuffer();

        void UpdateData(const void* data, uint64_t size, uint64_t startVertex = 0);

        uint64_t GetCount() const { return m_Count; }

        uint32_t GetStride() const { return m_Stride; }

        uint64_t GetSize() const { return m_Size; }

    protected:
        friend class Renderer;

        RVertexBuffer(Ref<RenderContex> contex, uint32_t stride, uint64_t size, const char* lable);

        static Ref<RVertexBuffer> Construct(Ref<RenderContex> contex, uint32_t stride, uint64_t size, const char* lable)
        {
            return std::shared_ptr<RVertexBuffer>(new RVertexBuffer(contex, stride, size, lable));
        }

        uint64_t m_Count = 0;
        uint32_t m_Stride = 0;
        uint64_t m_Size = 0;

        Ref<wgpu::Buffer> m_Buffer;
        Ref<wgpu::Queue> m_Queue;
    };

    class RIndexBuffer
    {
    public:
        ~RIndexBuffer();

        void UpdateData(const void* data, uint64_t size, uint32_t startIndex = 0);

        bool Is32Bits() const { return m_Use32Bits; }

        uint64_t GetCount() const { return m_Count; }

        uint64_t GetSize() const { return m_Size; }

    protected:
        friend class Renderer;

        RIndexBuffer(Ref<RenderContex> contex, uint64_t count, bool use32bits, const char* lable);

        static Ref<RIndexBuffer> Construct(Ref<RenderContex> contex, uint64_t count, bool use32bits, const char* lable)
        {
            return std::shared_ptr<RIndexBuffer>(new RIndexBuffer(contex, count, use32bits, lable));
        }

        uint64_t m_Count = 0;
        uint64_t m_Size = 0;
        bool m_Use32Bits = 0;

        Ref<wgpu::Buffer> m_Buffer;
        Ref<wgpu::Queue> m_Queue;
    };
}