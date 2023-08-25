#include "stdafx.h"
#include "render/RBuffer.h"
#include "dawn/webgpu.h"
#include "RenderContex.h"

namespace rush
{

    RVertexBuffer::RVertexBuffer(Ref<RenderContex> contex, uint32_t stride, uint64_t size, const char* lable)
    {
        m_Size = size;
        m_Stride = stride;
        m_Count = m_Size / m_Stride;
        RUSH_ASSERT(m_Size % 4 == 0);
        m_Buffer = CreateRef<wgpu::Buffer>();
        wgpu::BufferDescriptor descriptor;
        descriptor.label = lable;
        descriptor.size = m_Size;
        descriptor.usage = wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst;
        *m_Buffer = contex->device.CreateBuffer(&descriptor);
        m_Queue = contex->queue;
    }

    RVertexBuffer::~RVertexBuffer()
    {
        
    }

    void RVertexBuffer::UpdateData(const void* data, uint64_t size, uint64_t startVertex)
    {
        uint64_t offset = startVertex * m_Stride;
        if (data != nullptr && size > 0 && offset + size <= m_Size)
        {
            m_Queue->WriteBuffer(*m_Buffer.get(), offset, data, size);
        }
        else
        {
            LOG_ERROR("RVertexBuffer::UpdateData, size error {} {}", size, offset);
        }
    }

    //////////////////////////////////////////////////////////////////////////

    RIndexBuffer::RIndexBuffer(Ref<RenderContex> contex, uint64_t count, bool use32bits /*= false*/, const char* lable)
    {
        m_Use32Bits = use32bits;
        m_Count = count;
        m_Size = m_Count * (use32bits ? sizeof(uint32_t) : sizeof(uint16_t));
        RUSH_ASSERT(m_Size % 4 == 0);
        m_Buffer = CreateRef<wgpu::Buffer>();
        wgpu::BufferDescriptor descriptor;
        descriptor.label = lable;
        descriptor.size = m_Size;
        descriptor.usage = wgpu::BufferUsage::Index | wgpu::BufferUsage::CopyDst;
        *m_Buffer = contex->device.CreateBuffer(&descriptor);
        m_Queue = contex->queue;
    }

    RIndexBuffer::~RIndexBuffer()
    {

    }

    void RIndexBuffer::UpdateData(const void* data, uint64_t size, uint32_t startIndex /*= 0*/)
    {
        uint64_t offset = startIndex * (m_Use32Bits ? sizeof(uint32_t) : sizeof(uint16_t));
        if (data != nullptr && size > 0 && offset + size <= m_Size)
        {
            m_Queue->WriteBuffer(*m_Buffer.get(), offset, data, size);
        }
        else
        {
            LOG_ERROR("RVertexBuffer::UpdateData, size error {} {}", size, offset);
        }
    }


}