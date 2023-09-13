#include "stdafx.h"
#include "render/RBuffer.h"
#include "render/RenderContext.h"

namespace rush
{

    //////////////////////////////////////////////////////////////////////////

    RBuffer::RBuffer(wgpu::BufferUsage usage, uint64_t size, const void* data, const char* lable)
    {
        m_Size = size;
        wgpu::BufferDescriptor descriptor;
        descriptor.label = lable;
        descriptor.size = size;
        descriptor.usage = usage | wgpu::BufferUsage::CopyDst;
        m_Buffer = RenderContext::device.CreateBuffer(&descriptor);
        UpdateData(data, m_Size, 0);
    }

    bool RBuffer::IsValid() const
    {
        return (bool)m_Buffer;
    }

    void RBuffer::Destroy()
    {
        m_Buffer = {};
    }

    void RBuffer::UpdateData(const void* data, uint64_t size, uint64_t offset /*= 0*/)
    {
        if (data != nullptr)
        {
            if (size > 0 && offset + size <= m_Size)
            {
                RenderContext::queue.WriteBuffer(m_Buffer, offset, data, size);
            }
            else if (size <= 0)
            {
                RenderContext::queue.WriteBuffer(m_Buffer, offset, data, m_Size);
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////

    RVertexBuffer::RVertexBuffer(uint32_t stride, uint64_t size, const void* data, const char* lable)
        : RBuffer(wgpu::BufferUsage::Vertex, size, data, lable)
    {
        m_Stride = stride;
        m_VertCount = m_Size / m_Stride;
        RUSH_ASSERT(m_Size % 4 == 0);
    }

    //////////////////////////////////////////////////////////////////////////

    RIndexBuffer::RIndexBuffer(uint64_t count, IndexFormat type, const void* data /*= nullptr*/, const char* lable /*= nullptr*/)
        : RBuffer(wgpu::BufferUsage::Index, count * (type == IndexFormat::Uint32 ? sizeof(uint32_t) : sizeof(uint16_t)), data, lable)
    {
        m_Type = type;
        m_IndexCount = count;
        RUSH_ASSERT(m_Size % 4 == 0);
    }

    //////////////////////////////////////////////////////////////////////////

    RUniformBuffer::RUniformBuffer(uint64_t size, const void* data /*= nullptr*/, const char* lable /*= nullptr*/)
        : RBuffer(wgpu::BufferUsage::Uniform, size, data, lable)
    {
    }

    //////////////////////////////////////////////////////////////////////////

    RStorageBuffer::RStorageBuffer(uint64_t size, const void* data /*= nullptr*/, const char* lable /*= nullptr*/)
        : RBuffer(wgpu::BufferUsage::Storage, size, data, lable)
    {
    }

}