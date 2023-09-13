#ifndef RBuffer_h__
#define RBuffer_h__

#include "core/Core.h"
#include "render/RDefines.h"

namespace rush
{

    //////////////////////////////////////////////////////////////////////////

    class RBuffer
    {
    public:
        virtual ~RBuffer() = default;
        RBuffer(const RBuffer&) = delete;
        RBuffer& operator=(const RBuffer&) = delete;

        bool IsValid() const;
        void Destroy();

        const wgpu::Buffer& GetBuffer() const { return m_Buffer; }

        void UpdateData(const void* data, uint64_t size = 0, uint64_t offset = 0);

        uint64_t GetSize() const { return m_Size; }

    protected:
        RBuffer(wgpu::BufferUsage usage, uint64_t size, const void* data, const char* lable);

    protected:
        friend class RenderContext;
        uint64_t m_Size = 0;
        wgpu::Buffer m_Buffer;
    };

    //////////////////////////////////////////////////////////////////////////

    class RUniformBuffer : public RBuffer
    {
    public:
        RUniformBuffer(uint64_t size, const void* data = nullptr, const char* lable = nullptr);

    };

    //////////////////////////////////////////////////////////////////////////
    class RStorageBuffer : public RBuffer
    {
    public:
        RStorageBuffer(uint64_t size, const void* data = nullptr, const char* lable = nullptr);

    };

    //////////////////////////////////////////////////////////////////////////

    class RVertexBuffer : public RBuffer
    {
    public:
        RVertexBuffer(uint32_t stride, uint64_t size, const void* data = nullptr, const char* lable = nullptr);

        uint64_t GetVertCount() const { return m_VertCount; }

        uint32_t GetStride() const { return m_Stride; }

        uint64_t GetSize() const { return m_Size; }

    protected:
        friend class RenderContext;

        uint64_t m_VertCount = 0;
        uint32_t m_Stride = 0;
    };


    //////////////////////////////////////////////////////////////////////////

    class RIndexBuffer : public RBuffer
    {
    public:
        RIndexBuffer(uint64_t count, IndexFormat type, const void* data = nullptr, const char* lable = nullptr);

        IndexFormat GetType() const { return m_Type; }

        uint64_t GetIndexCount() const { return m_IndexCount; }

        uint64_t GetSize() const { return m_Size; }

    protected:
        friend class RenderContext;

        uint64_t m_IndexCount = 0;
        IndexFormat m_Type = IndexFormat::Uint16;
    };

}

#endif // RBuffer_h__
